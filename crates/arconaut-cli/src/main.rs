mod repl;

use arconaut_machine::AnthropicProvider;
use repl::Repl;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let api_key = std::env::var("ANTHROPIC_API_KEY")
        .map_err(|_| "error: ANTHROPIC_API_KEY environment variable not set")?;

    let provider = AnthropicProvider::new(api_key)?;
    let mut repl = Repl::new(Box::new(provider));

    repl.run().await?;
    Ok(())
}
