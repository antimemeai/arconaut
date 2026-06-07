#![allow(dead_code)]

use arconaut_core::{Context, Message};
use arconaut_machine::{ChatProvider, ChatRequest};
use std::io::{self, Write};

pub struct Repl {
    provider: Box<dyn ChatProvider>,
    context: Context,
}

impl Repl {
    pub fn new(provider: Box<dyn ChatProvider>) -> Self {
        Self {
            provider,
            context: Context::new(200_000),
        }
    }

    pub async fn run(&mut self) -> io::Result<()> {
        println!("arconaut v0.1.0 — type /quit to exit\n");

        loop {
            print!("> ");
            io::stdout().flush()?;

            let mut input = String::new();
            io::stdin().read_line(&mut input)?;
            let input = input.trim();

            if input.is_empty() {
                continue;
            }

            if let Some(command) = parse_command(input) {
                match command {
                    Command::Quit => {
                        println!("goodbye");
                        return Ok(());
                    }
                }
            } else {
                match self.chat(input).await {
                    Ok(response) => println!("{}\n", response),
                    Err(e) => eprintln!("error: {}\n", e),
                }
            }
        }
    }

    async fn chat(&mut self, input: &str) -> Result<String, String> {
        let message = Message::user(input);
        self.context.append_message(message);

        let request = ChatRequest {
            messages: self.context.history().to_vec(),
            tools: vec![],
            system_prompt: None,
        };

        match self.provider.chat(request).await {
            Ok(response) => {
                let text: String = response
                    .message
                    .content
                    .iter()
                    .filter_map(|part| match part {
                        arconaut_core::ContentPart::Text { text } => Some(text.as_str()),
                        _ => None,
                    })
                    .collect();

                self.context.append_message(response.message);
                Ok(text)
            }
            Err(e) => Err(e.to_string()),
        }
    }
}

#[derive(Debug, Clone, PartialEq)]
enum Command {
    Quit,
}

fn parse_command(input: &str) -> Option<Command> {
    if input == "/quit" {
        Some(Command::Quit)
    } else {
        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parse_quit_command() {
        assert_eq!(parse_command("/quit"), Some(Command::Quit));
    }

    #[test]
    fn parse_non_command() {
        assert_eq!(parse_command("hello"), None);
        assert_eq!(parse_command("/quit now"), None);
        assert_eq!(parse_command("quit"), None);
    }
}
