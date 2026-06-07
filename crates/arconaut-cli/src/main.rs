mod repl;

use arconaut_machine::AnthropicProvider;
use repl::Repl;

#[tokio::main]
async fn main() {
    let api_key = std::env::var("ANTHROPIC_API_KEY").unwrap_or_else(|_| {
        eprintln!("error: ANTHROPIC_API_KEY environment variable not set");
        std::process::exit(1);
    });

    let provider = AnthropicProvider::new(api_key);
    let mut repl = Repl::new(provider);

    if let Err(e) = repl.run().await {
        eprintln!("repl error: {}", e);
        std::process::exit(1);
    }
}
