#[cfg(test)]
mod repl;
mod terminal_bridge;
mod utils;

use arconaut_agent::{PersistentShell, Soul};
use arconaut_core::{ToolRegistry, VariableStore};
use arconaut_machine::{
    skills::{SkillLoader, SkillTool},
    tools::{BashTool, EditTool, GrepTool, ReadTool, WriteTool},
    AnthropicProvider,
};
use arconaut_tui::{ghostty, App, SoulCommand, TuiEvent};
use crossterm::{
    event::{DisableMouseCapture, EnableMouseCapture},
    terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen},
    ExecutableCommand,
};
use std::io;
use std::path::PathBuf;
use tokio::sync::mpsc;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
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

    let soul_handle = tokio::spawn(run_soul(soul_rx, tui_tx));

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
