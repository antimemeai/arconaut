#[cfg(test)]
mod repl;
mod terminal_bridge;
mod utils;

use arconaut_agent::{AgentMode, Bus, InboxServer, PersistentShell, Session, Soul};
use arconaut_core::{ToolRegistry, VariableStore};
use arconaut_machine::{
    auth::{CredentialStorage, FileStorage, KimiOAuthFlow, ModelInfo, OAuthError},
    skills::{SkillLoader, SkillTool},
    tools::{BashTool, EditTool, GrepTool, ReadTool, WriteTool},
    ProviderFactory,
};
use arconaut_tui::{ghostty, App, SoulCommand, TuiEvent};
use clap::{Parser, Subcommand};
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
#[command(args_conflicts_with_subcommands = true)]
struct Cli {
    #[command(flatten)]
    run: RunArgs,

    #[command(subcommand)]
    command: Option<Commands>,
}

#[derive(Subcommand, Debug)]
enum Commands {
    /// Run the agent (default behavior)
    Run(RunArgs),
    /// Login to an LLM provider via OAuth
    Login(LoginArgs),
    /// Logout from an LLM provider
    Logout(LogoutArgs),
}

#[derive(Parser, Debug)]
struct RunArgs {
    /// Agent name to use for this session.
    #[arg(long, short)]
    agent: Option<String>,

    /// Session name.
    #[arg(long, short)]
    session: Option<String>,

    /// Agent mode (implement, review, explore, test, assist).
    #[arg(long, short)]
    mode: Option<String>,

    /// LLM provider name (must be configured in vars.toml).
    #[arg(long, short)]
    provider: Option<String>,

    /// Override the model for the selected provider.
    #[arg(long)]
    model: Option<String>,

    /// Assistant provider name for secondary model.
    #[arg(long)]
    assistant_provider: Option<String>,

    /// Run one turn and exit (no TUI).
    #[arg(long)]
    no_tui: bool,

    /// Input text for single-turn mode.
    #[arg(trailing_var_arg = true)]
    input: Vec<String>,
}

#[derive(Parser, Debug)]
struct LoginArgs {
    /// Provider to log in to (e.g., kimi).
    provider: String,

    /// Do not open a browser window.
    #[arg(long)]
    no_browser: bool,
}

#[derive(Parser, Debug)]
struct LogoutArgs {
    /// Provider to log out from (e.g., kimi).
    provider: String,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let cli = Cli::parse();

    match cli.command {
        None => run_main(cli.run).await,
        Some(Commands::Run(args)) => run_main(args).await,
        Some(Commands::Login(args)) => run_login(args).await,
        Some(Commands::Logout(args)) => run_logout(args).await,
    }
}

async fn run_main(args: RunArgs) -> Result<(), Box<dyn std::error::Error>> {
    let agent_name = args.agent.unwrap_or_else(|| "default".to_string());
    let session_name = args.session.unwrap_or_else(|| "main".to_string());
    let _session = Session::new(&session_name, &agent_name);

    let mode = args
        .mode
        .as_deref()
        .and_then(parse_mode)
        .unwrap_or(AgentMode::Assist);

    let provider_name = args.provider.unwrap_or_else(|| "anthropic".to_string());
    let model_override = args.model;
    let assistant_provider = args.assistant_provider;

    if args.no_tui {
        run_single_turn(
            &agent_name,
            mode,
            &args.input,
            &provider_name,
            model_override.as_deref(),
            assistant_provider.as_deref(),
        )
        .await?;
        return Ok(());
    }

    run_tui(
        &agent_name,
        mode,
        &provider_name,
        model_override.as_deref(),
        assistant_provider.as_deref(),
    )
    .await
}

async fn run_login(args: LoginArgs) -> Result<(), Box<dyn std::error::Error>> {
    match args.provider.as_str() {
        "kimi" | "moonshot" => login_kimi(args.no_browser).await,
        other => {
            eprintln!("Unsupported provider for OAuth login: {}", other);
            std::process::exit(1);
        }
    }
}

/// Simple terminal spinner for CLI progress indication.
struct Spinner {
    frames: [&'static str; 10],
    idx: usize,
}

impl Spinner {
    fn new() -> Self {
        Self {
            frames: ["⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"],
            idx: 0,
        }
    }

    fn tick(&mut self) -> &'static str {
        let frame = self.frames[self.idx];
        self.idx = (self.idx + 1) % self.frames.len();
        frame
    }
}

async fn login_kimi(no_browser: bool) -> Result<(), Box<dyn std::error::Error>> {
    let flow = KimiOAuthFlow::new();
    let mut spinner = Spinner::new();

    println!("Starting Kimi OAuth login...");

    let auth = flow.request_device_authorization().await?;

    println!("Please visit the following URL to authorize arconaut:");
    println!("  {}", auth.verification_uri_complete);
    println!();

    if !no_browser {
        print!("Opening browser... ");
        io::Write::flush(&mut io::stdout())?;
        #[cfg(target_os = "macos")]
        {
            let _ = std::process::Command::new("open")
                .arg(&auth.verification_uri_complete)
                .spawn();
        }
        println!("✓");
    }

    let interval = std::time::Duration::from_secs(auth.interval.max(1));
    let max_poll_duration = std::time::Duration::from_secs(600); // 10 minutes
    let poll_start = std::time::Instant::now();
    let mut expiry_restart = false;
    let token;

    print!("Waiting for authorization... ");
    io::Write::flush(&mut io::stdout())?;

    loop {
        tokio::time::sleep(interval).await;

        if poll_start.elapsed() > max_poll_duration {
            println!();
            eprintln!("Authorization timed out. Please try again.");
            std::process::exit(1);
        }

        match flow.poll_token(&auth).await {
            Ok(t) => {
                token = t;
                break;
            }
            Err(OAuthError::PollPending) => {
                print!("\rWaiting for authorization... {} ", spinner.tick());
                io::Write::flush(&mut io::stdout())?;
            }
            Err(OAuthError::DeviceExpired) if !expiry_restart => {
                println!();
                println!("Device code expired. Requesting new code...");
                expiry_restart = true;
                // Restart the device auth flow once
                let new_auth = flow.request_device_authorization().await?;
                println!("Please visit the following URL to authorize arconaut:");
                println!("  {}", new_auth.verification_uri_complete);
                print!("Waiting for authorization... ");
                io::Write::flush(&mut io::stdout())?;
                continue;
            }
            Err(OAuthError::DeviceExpired) => {
                println!();
                eprintln!("Device authorization expired. Please try again.");
                std::process::exit(1);
            }
            Err(e) => {
                println!();
                eprintln!("OAuth error: {}", e);
                std::process::exit(1);
            }
        }
    }

    println!("\rWaiting for authorization... ✓");

    // Save token
    let storage = FileStorage::new()?;
    storage.save("kimi", &token)?;
    println!("✓ Saved credentials");

    // Fetch models
    let models = match flow.fetch_models(&token.access_token).await {
        Ok(m) => {
            println!("✓ Fetched {} model(s)", m.len());
            m
        }
        Err(e) => {
            eprintln!("⚠ Failed to fetch models: {}", e);
            Vec::new()
        }
    };

    // Auto-update vars.toml
    if let Ok(home) = std::env::var("HOME") {
        let config_dir = PathBuf::from(home).join(".config").join("arconaut");
        let vars_path = config_dir.join("vars.toml");
        if let Err(e) = update_vars_toml(&vars_path, &models) {
            eprintln!("⚠ Failed to update vars.toml: {}", e);
        } else {
            println!("✓ Updated {}", vars_path.display());
        }
    }

    println!();
    println!("Logged in to Kimi successfully.");

    Ok(())
}

/// Update vars.toml to enable OAuth for the moonshot provider.
/// Preserves all existing config. Backs up the old file.
fn update_vars_toml(
    vars_path: &std::path::Path,
    models: &[ModelInfo],
) -> Result<(), Box<dyn std::error::Error>> {
    use std::io::Write;

    let config_dir = vars_path.parent().unwrap_or(std::path::Path::new("."));
    std::fs::create_dir_all(config_dir)?;

    // Backup existing file
    if vars_path.exists() {
        let timestamp = chrono::Utc::now().format("%Y%m%d%H%M%S");
        let backup = config_dir.join(format!("vars.toml.bak.{}", timestamp));
        std::fs::copy(vars_path, &backup)?;
    }

    // Read existing content
    let mut table = if vars_path.exists() {
        let content = std::fs::read_to_string(vars_path)?;
        content.parse::<toml::Table>()?
    } else {
        toml::Table::new()
    };

    // Update provider.moonshot section
    let provider = table
        .entry("provider")
        .or_insert_with(|| toml::Value::Table(toml::Table::new()));

    if let toml::Value::Table(provider_table) = provider {
        let moonshot = provider_table
            .entry("moonshot")
            .or_insert_with(|| toml::Value::Table(toml::Table::new()));

        if let toml::Value::Table(moonshot_table) = moonshot {
            moonshot_table.insert("oauth".to_string(), toml::Value::Boolean(true));
            moonshot_table
                .entry("api_key")
                .or_insert_with(|| toml::Value::String("".to_string()));

            // Set model to the first available model, or keep existing
            if let Some(first) = models.first() {
                moonshot_table.insert(
                    "model".to_string(),
                    toml::Value::String(first.id.clone()),
                );
            } else {
                moonshot_table
                    .entry("model")
                    .or_insert_with(|| toml::Value::String("moonshot-v1-8k".to_string()));
            }
        }
    }

    // Atomic write
    let tmp = vars_path.with_extension("tmp");
    {
        let mut file = std::fs::File::create(&tmp)?;
        file.write_all(table.to_string().as_bytes())?;
        file.sync_all()?;
    }
    std::fs::rename(&tmp, vars_path)?;

    Ok(())
}

async fn run_logout(args: LogoutArgs) -> Result<(), Box<dyn std::error::Error>> {
    match args.provider.as_str() {
        "kimi" | "moonshot" => logout_kimi().await,
        other => {
            eprintln!("Unsupported provider for OAuth logout: {}", other);
            std::process::exit(1);
        }
    }
}

async fn logout_kimi() -> Result<(), Box<dyn std::error::Error>> {
    let storage = FileStorage::new()?;
    storage.delete("kimi")?;
    println!("✓ Logged out from Kimi. OAuth credentials cleared.");
    Ok(())
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
    provider_name: &str,
    model_override: Option<&str>,
    assistant_provider: Option<&str>,
) -> Result<(), Box<dyn std::error::Error>> {
    let _ = (agent_name, mode); // TODO: wire into system prompt

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

    let vars = load_vars().await;
    let provider = build_provider(provider_name, model_override, &vars)
        .unwrap_or_else(|| Box::new(arconaut_agent::MockProvider::new(vec![])));

    let mut _refresh_task = provider.start_refresh_task();

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

    // Setup tool registry (same as TUI minus terminal bridge and bus).
    let mut registry = ToolRegistry::new();
    registry.register(Box::new(ReadTool::new()));
    registry.register(Box::new(WriteTool::new()));
    registry.register(Box::new(EditTool::new()));
    registry.register(Box::new(BashTool::new()));
    registry.register(Box::new(GrepTool::new()));
    registry.register(Box::new(SkillTool::new(skill_loader)));
    for tool in utils::UtilsBin::tools() {
        registry.register(tool);
    }

    // Setup audit logging.
    let audit_dir = home
        .as_ref()
        .map(|h| h.join(".local").join("share").join("arconaut").join("audit"))
        .unwrap_or_else(|| std::env::current_dir().unwrap().join(".arconaut").join("audit"));

    // Setup Soul with same configuration as TUI mode.
    let mut soul = Soul::new(provider, registry)
        .with_max_steps(50)
        .with_context_size(200_000)
        .with_intervention(arconaut_agent::InterventionInjector::new());

    if let Ok(logger) = arconaut_audit::AuditLogger::new("default", audit_dir) {
        soul.hook_engine_mut()
            .register(Box::new(arconaut_agent::AuditHook::new(logger)));
    }

    // Setup assistant model if configured.
    if let Some(assistant_name) = assistant_provider {
        if let Some(assistant_provider) = build_provider(assistant_name, None, &vars) {
            let assistant = arconaut_agent::AssistantModel::new(assistant_provider);
            soul = soul.with_assistant(assistant);
        }
    }

    match soul.run_turn(&text).await {
        Ok(result) => println!("{}", format_message(&result.message)),
        Err(e) => eprintln!("error: {}", e),
    }

    drop(_refresh_task);
    Ok(())
}

async fn run_tui(
    agent_name: &str,
    mode: AgentMode,
    provider_name: &str,
    model_override: Option<&str>,
    assistant_provider: Option<&str>,
) -> Result<(), Box<dyn std::error::Error>> {
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

    let soul_handle = tokio::spawn(run_soul(
        soul_rx,
        tui_tx,
        bus,
        provider_name.to_string(),
        model_override.map(|s| s.to_string()),
        assistant_provider.map(|s| s.to_string()),
    ));

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
    provider_name: String,
    model_override: Option<String>,
    assistant_provider: Option<String>,
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

    // Setup primary provider.
    let provider = build_provider(&provider_name, model_override.as_deref(), &vars)
        .unwrap_or_else(|| {
            let _ = tui_tx.try_send(TuiEvent::Status(
                format!("Provider '{}' not configured — running in echo mode", provider_name),
            ));
            Box::new(arconaut_agent::MockProvider::new(vec![]))
        });

    let mut _refresh_task = provider.start_refresh_task();

    // Setup tool registry.
    let mut registry = ToolRegistry::new();
    registry.register(Box::new(ReadTool::new()));
    registry.register(Box::new(WriteTool::new()));
    registry.register(Box::new(EditTool::new()));
    registry.register(Box::new(BashTool::new()));
    registry.register(Box::new(GrepTool::new()));
    registry.register(Box::new(terminal_bridge::TerminalBridge::new(bridge_tx)));
    registry.register(Box::new(SkillTool::new(skill_loader)));
    registry.register(Box::new(arconaut_agent::BusTool::new(Arc::clone(&bus))));
    for tool in utils::UtilsBin::tools() {
        registry.register(tool);
    }

    // Setup audit logging.
    let audit_dir = home
        .as_ref()
        .map(|h| h.join(".local").join("share").join("arconaut").join("audit"))
        .unwrap_or_else(|| std::env::current_dir().unwrap().join(".arconaut").join("audit"));

    // Setup off-pulse intervention.
    let mut soul = Soul::new(provider, registry)
        .with_max_steps(50)
        .with_context_size(200_000)
        .with_intervention(arconaut_agent::InterventionInjector::new());

    if let Ok(logger) = arconaut_audit::AuditLogger::new("default", audit_dir) {
        soul.hook_engine_mut()
            .register(Box::new(arconaut_agent::AuditHook::new(logger)));
    }

    // Setup assistant model if configured.
    if let Some(assistant_name) = assistant_provider {
        if let Some(assistant_provider) = build_provider(&assistant_name, None, &vars) {
            let assistant = arconaut_agent::AssistantModel::new(assistant_provider)
                .with_bus(Arc::clone(&bus));
            soul = soul.with_assistant(assistant);
        }
    }

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

fn format_message(msg: &arconaut_core::Message) -> String {
    msg.content
        .iter()
        .filter_map(|part| part.as_text())
        .collect::<Vec<_>>()
        .join("")
}

/// Load system and project variables.
async fn load_vars() -> VariableStore {
    let mut vars = VariableStore::new();
    if let Ok(home) = std::env::var("HOME") {
        vars.load_system(PathBuf::from(home).join(".config").join("arconaut").join("vars.toml"))
            .await;
    }
    let project_vars_path = std::env::current_dir()
        .unwrap_or_default()
        .join(".arconaut")
        .join("vars.toml");
    vars.load_project(project_vars_path).await;
    vars
}

/// Build a provider from config, with optional model override.
fn build_provider(
    name: &str,
    model_override: Option<&str>,
    vars: &VariableStore,
) -> Option<Box<dyn arconaut_machine::ChatProvider>> {
    let mut provider = ProviderFactory::create_named(name, vars).ok()?;
    if let Some(model) = model_override {
        // Model override requires provider-specific handling.
        // For now, we re-create the provider with the overridden model.
        let cfg = vars.get_prefixed(&format!("provider.{}", name));
        let kind = match name.to_lowercase().as_str() {
            "anthropic" => arconaut_machine::ProviderKind::Anthropic,
            "openai" => arconaut_machine::ProviderKind::OpenAi,
            "gemini" => arconaut_machine::ProviderKind::Gemini,
            "moonshot" => arconaut_machine::ProviderKind::Moonshot,
            "openrouter" => arconaut_machine::ProviderKind::OpenRouter,
            _ => return Some(provider),
        };
        let api_key = cfg.get("api_key")?.as_str()?.to_string();
        let base_url = cfg.get("base_url").and_then(|v| v.as_str()).map(|s| s.to_string());
        let oauth = cfg.get("oauth").and_then(|v| v.as_bool()).unwrap_or(false);
        let new_cfg = arconaut_machine::ProviderConfig {
            kind,
            api_key,
            model: model.to_string(),
            base_url,
            extra_headers: None,
            oauth,
        };
        provider = ProviderFactory::create(&new_cfg).ok()?;
    }
    Some(provider)
}
