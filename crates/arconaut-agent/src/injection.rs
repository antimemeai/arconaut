use arconaut_core::{Context, Message};

/// Injects content into the conversation context at turn start.
pub trait Injector: Send + Sync {
    fn inject(&self, context: &mut Context);
}

/// Injects a fixed system prompt at the beginning of context.
pub struct SystemPromptInjector {
    prompt: String,
}

impl SystemPromptInjector {
    pub fn new(prompt: impl Into<String>) -> Self {
        Self {
            prompt: prompt.into(),
        }
    }
}

impl Injector for SystemPromptInjector {
    fn inject(&self, context: &mut Context) {
        // Check if this exact prompt already exists to avoid duplicates
        let already_present = context.history().iter().any(|m| {
            m.role == arconaut_core::Role::System
                && m.content.iter().any(|p| {
                    p.as_text().map(|t| t.trim() == self.prompt.trim()).unwrap_or(false)
                })
        });
        if !already_present {
            // Insert after the last existing system message so composed
            // injectors maintain their declared order.
            let pos = context
                .history()
                .iter()
                .enumerate()
                .rev()
                .find(|(_, m)| m.role == arconaut_core::Role::System)
                .map(|(i, _)| i + 1)
                .unwrap_or(0);
            context.insert_message(pos, Message::system(&self.prompt));
        }
    }
}

/// Combines multiple injectors, running them in order.
pub struct CompositeInjector {
    injectors: Vec<Box<dyn Injector>>,
}

impl CompositeInjector {
    pub fn new() -> Self {
        Self {
            injectors: Vec::new(),
        }
    }

    pub fn add(&mut self, injector: Box<dyn Injector>) {
        self.injectors.push(injector);
    }
}

impl Default for CompositeInjector {
    fn default() -> Self {
        Self::new()
    }
}

impl Injector for CompositeInjector {
    fn inject(&self, context: &mut Context) {
        for injector in &self.injectors {
            injector.inject(context);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn system_prompt_injected_at_head() {
        let injector = SystemPromptInjector::new("you are helpful");
        let mut ctx = Context::new(200_000);
        ctx.append_message(Message::user("hello"));

        injector.inject(&mut ctx);
        assert_eq!(ctx.history()[0].role, arconaut_core::Role::System);
        assert_eq!(
            ctx.history()[0].content[0].as_text().unwrap(),
            "you are helpful"
        );
    }

    #[test]
    fn system_prompt_not_duplicated() {
        let injector = SystemPromptInjector::new("you are helpful");
        let mut ctx = Context::new(200_000);
        ctx.append_message(Message::system("you are helpful"));

        injector.inject(&mut ctx);
        assert_eq!(ctx.history().len(), 1);
    }

    #[test]
    fn composition_order() {
        let mut composite = CompositeInjector::new();
        composite.add(Box::new(SystemPromptInjector::new("first")));
        composite.add(Box::new(SystemPromptInjector::new("second")));

        let mut ctx = Context::new(200_000);
        composite.inject(&mut ctx);

        // Both system prompts injected in order; duplicates skipped
        assert_eq!(ctx.history().len(), 2);
        assert_eq!(ctx.history()[0].content[0].as_text().unwrap(), "first");
        assert_eq!(ctx.history()[1].content[0].as_text().unwrap(), "second");
    }

    #[test]
    fn duplicate_prompt_skipped() {
        let mut composite = CompositeInjector::new();
        composite.add(Box::new(SystemPromptInjector::new("same")));
        composite.add(Box::new(SystemPromptInjector::new("same")));

        let mut ctx = Context::new(200_000);
        composite.inject(&mut ctx);

        assert_eq!(ctx.history().len(), 1);
    }
}
