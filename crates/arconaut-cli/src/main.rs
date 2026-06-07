#[cfg(test)]
mod repl;
mod terminal_bridge;
mod utils;

use arconaut_agent::{AgentMode, Bus, InboxServer, PersistentShell, Session, Soul};
use arconaut_core::{ToolRegistry, VariableStore};
use arconaut_machine::{
    skills::{SkillLoader, SkillTool},
    tools::{BashTool, EditTool, GrepTool, ReadTool, WriteTool},
    AnthropicProvider,
};
use arconaut_tui::{ghostty, App, SoulCommand, TuiEvent};
use clap::Parser;
use crossterm::{
    event::{DisableMouseCapture, EnableMouseCapture},
    terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen},
    ExecutableCommand,
};
use std::io;
use std::path::PathBuf;
use std::sync::Arc;
use tokio::sync::mpsc;

#[derive(Parser, Debug)]
#[command(name = "arconaut")]
#[command(about = "AI-native dev environment")]
struct Args {
    /// Agent name to use for this session.
    #[arg(long, short)]
    agent: Option<String>,

    /// Session name.
    #[arg(long, short)]
    session: Option<String>,

    /// Agent mode (implement, review, explore, test, assist).
    #[arg(long, short)]
    mode: Option<String>,

    /// Run one turn and exit (no TUI).
    #[arg(long)]
    no_tui: bool,

    /// Input text for single-turn mode.
    #[arg(trailing_var_arg = true)]
    input: Vec<String>,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();

    let agent_name = args.agent.unwrap_or_else(|| "default".to_string());
    let session_name = args.session.unwrap_or_else(|| "main".to_string());
    let _session = Session::new(&session_name, &agent_name);

    let mode = args
        .mode
        .as_deref()
        .and_then(parse_mode)
        .unwrap_or(AgentMode::Assist);

    if args.no_tui {
        run_single_turn(&agent_name, mode, &args.input).await?;
        return Ok(());
    }

    run_tui(&agent_name, mode).await
}

fn parse_mode(s: &str) -> Option<AgentMode> {
    match s.to_lowercase().as_str() {
        "implement" | "impl" | "code" => Some(AgentMode::Implement),
        "review" | "rev" => Some(AgentMode::Review),
        "explore" | "research" => Some(AgentMode::Explore),
        "test" => Some(AgentMode::Test),
        "assist" | "help" => Some(AgentMode::Assist),
        _ => None,
    }
}

async fn run_single_turn(
    agent_name: &str,
    mode: AgentMode,
    input: &[String],
) -> Result<(), Box<dyn std::error::Error>> {
    let text = if input.is_empty() {
        use std::io::{BufRead, Write};
        print!("{}[{:?}]> ", agent_name, mode);
        io::stdout().flush()?;
        let mut line = String::new();
        io::stdin().lock().read_line(&mut line)?;
        line.trim().to_string()
    } else {
        input.join(" ")
    };

    if text.is_empty() {
        return Ok(());
    }

    let provider: Box<dyn arconaut_machine::ChatProvider> =
        match std::env::var("ANTHROPIC_API_KEY") {
            Ok(key) => Box::new(AnthropicProvider::new(key)?),
            Err(_) => Box::new(arconaut_agent::MockProvider::new(vec![])),
        };

    let registry = default_registry().await;
    let mut soul = Soul::new(provider, registry);

    match soul.run_turn(&text).await {
        Ok(result) => println!("{}", format_message(&result.message)),
        Err(e) => eprintln!("error: {}", e),
    }

    Ok(())
}

async fn run_tui(agent_name: &str, mode: AgentMode) -> Result<(), Box<dyn std::error::Error>> {
    let _ = (agent_name, mode); // TODO: wire into TUI title/status

    // Start agent bus and gRPC inbox server.
    let bus = Arc::new(Bus::new());
    let inbox = InboxServer::new();
    let inbox_addr: std::net::SocketAddr = "127.0.0.1:50051".parse()?;
    tokio::spawn(async move {
        let _ = inbox.start(inbox_addr).await;
    });

    enable_raw_mode()?;
    let _ = ghostty::push_keyboard_flags();
    ghostty::query_theme();

    let mut stdout = io::stdout();
    stdout.execute(EnterAlternateScreen)?;
    stdout.execute(EnableMouseCapture)?;

    let backend = ratatui::backend::CrosstermBackend::new(stdout);
    let mut terminal = ratatui::Terminal::new(backend)?;
    let size = terminal.size()?;

    let (soul_tx, soul_rx) = mpsc::channel::<SoulCommand>(100);
    let (tui_tx, tui_rx) = mpsc::channel::<TuiEvent>(100);

    let soul_handle = tokio::spawn(run_soul(soul_rx, tui_tx, bus));

    let mut app = App::new(soul_tx, tui_rx, size.height, size.width);
    let result = app.run(&mut terminal).await;

    let _ = ghostty::pop_keyboard_flags();
    disable_raw_mode()?;
    drop(terminal);
    let mut stdout = io::stdout();
    stdout.execute(LeaveAlternateScreen)?;
    stdout.execute(DisableMouseCapture)?;

    result?;

    // Give the soul task a moment to shut down gracefully.
    let _ = soul_handle.await;
    Ok(())
}

async fn run_soul(
    mut soul_rx: mpsc::Receiver<SoulCommand>,
    tui_tx: mpsc::Sender<TuiEvent>,
    bus: Arc<Bus>,
) {
    // Setup persistent shell.
    let (shell_out_tx, mut shell_out_rx) = mpsc::channel::<String>(100);
    let mut shell = match PersistentShell::new(shell_out_tx).await {
        Ok(s) => s,
        Err(e) => {
            let _ = tui_tx
                .send(TuiEvent::Error {
                    message: format!("failed to start shell: {}", e),
                })
                .await;
            return;
        }
    };

    // Channel for the terminal bridge tool to send input to the shell.
    let (bridge_tx, mut bridge_rx) =
        mpsc::channel::<(String, tokio::sync::oneshot::Sender<String>)>(100);

    // Setup skill loader.
    let home = std::env::var("HOME").map(PathBuf::from).ok();
    let user_skills_dir = home
        .as_ref()
        .map(|h| h.join(".config").join("arconaut").join("skills"))
        .unwrap_or_else(|| std::env::current_dir().unwrap().join(".arconaut").join("skills"));
    let project_skills_dir = std::env::current_dir()
        .unwrap_or_default()
        .join(".arconaut")
        .join("skills");
    let skill_loader = std::sync::Arc::new(SkillLoader::new(user_skills_dir, project_skills_dir));

    // Setup variable store.
    let mut vars = VariableStore::new();
    if let Some(ref home) = home {
        vars.load_system(home.join(".config").join("arconaut").join("vars.toml")).await;
    }
    let project_vars_path = std::env::current_dir()
        .unwrap_or_default()
        .join(".arconaut")
        .join("vars.toml");
    vars.load_project(project_vars_path).await;

    // Setup provider.
    let provider: Box<dyn arconaut_machine::ChatProvider> =
        match std::env::var("ANTHROPIC_API_KEY") {
            Ok(key) => match AnthropicProvider::new(key) {
                Ok(p) => Box::new(p),
                Err(e) => {
                    let _ = tui_tx
                        .send(TuiEvent::Error {
                            message: format!("Anthropic provider error: {}", e),
                        })
                        .await;
                    return;
                }
            },
            Err(_) => {
                let _ = tui_tx
                    .send(TuiEvent::Status(
                        "ANTHROPIC_API_KEY not set — running in echo mode".to_string(),
                    ))
                    .await;
                Box::new(arconaut_agent::MockProvider::new(vec![]))
            }
        };

    // Setup tool registry.
    let mut registry = ToolRegistry::new();
    registry.register(Box::new(ReadTool::new()));
    registry.register(Box::new(WriteTool::new()));
    registry.register(Box::new(EditTool::new()));
    registry.register(Box::new(BashTool::new()));
    registry.register(Box::new(GrepTool::new()));
    registry.register(Box::new(terminal_bridge::TerminalBridge::new(bridge_tx)));
    registry.register(Box::new(SkillTool::new(skill_loader)));
    registry.register(Box::new(arconaut_agent::BusTool::new(bus)));
    for tool in utils::UtilsBin::tools() {
        registry.register(tool);
    }

    let mut soul = Soul::new(provider, registry)
        .with_max_steps(50)
        .with_context_size(200_000);

    loop {
        tokio::select! {
            Some(line) = shell_out_rx.recv() => {
                let _ = tui_tx.send(TuiEvent::ShellOutput(line)).await;
            }
            Some((input, reply_tx)) = bridge_rx.recv() => {
                if let Err(e) = shell.send(&input).await {
                    let _ = reply_tx.send(format!("shell error: {}", e));
                } else {
                    tokio::time::sleep(tokio::time::Duration::from_millis(200)).await;
                    let output = shell.take_buffer();
                    let _ = reply_tx.send(output);
                }
            }
            Some(cmd) = soul_rx.recv() => {
                match cmd {
                    SoulCommand::UserInput(text) => {
                        let _ = tui_tx.send(TuiEvent::Status("Thinking...".to_string())).await;
                        match soul.run_turn(&text).await {
                            Ok(result) => {
                                let _ = tui_tx.send(TuiEvent::AssistantMessage(result.message)).await;
                                let _ = tui_tx.send(TuiEvent::TurnComplete).await;
                            }
                            Err(e) => {
                                let _ = tui_tx.send(TuiEvent::Error {
                                    message: e.to_string(),
                                }).await;
                            }
                        }
                    }
                    SoulCommand::TerminalOutput(text) => {
                        if let Err(e) = shell.send(&text).await {
                            let _ = tui_tx.send(TuiEvent::Error {
                                message: format!("shell error: {}", e),
                            }).await;
                        }
                    }
                    SoulCommand::Interrupt => {
                        let _ = tui_tx.send(TuiEvent::Status("Interrupted".to_string())).await;
                    }
                    SoulCommand::Quit => break,
                    SoulCommand::Resize(_, _) => {}
                }
            }
            else => break,
        }
    }
}

async fn default_registry() -> ToolRegistry {
    let mut registry = ToolRegistry::new();
    registry.register(Box::new(ReadTool::new()));
    registry.register(Box::new(WriteTool::new()));
    registry.register(Box::new(EditTool::new()));
    registry.register(Box::new(BashTool::new()));
    registry.register(Box::new(GrepTool::new()));
    for tool in utils::UtilsBin::tools() {
        registry.register(tool);
    }
    registry
}

fn format_message(msg: &arconaut_core::Message) -> String {
    msg.content
        .iter()
        .filter_map(|part| part.as_text())
        .collect::<Vec<_>>()
        .join("")
}
