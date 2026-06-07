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
        // Only inject if no system message already present at head
        if context
            .history()
            .first()
            .is_none_or(|m| m.role != arconaut_core::Role::System)
        {
            context.insert_message(0, Message::system(&self.prompt));
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
        ctx.append_message(Message::system("existing"));

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

        // Only first system prompt injected; second sees existing system and skips
        assert_eq!(ctx.history().len(), 1);
        assert_eq!(ctx.history()[0].content[0].as_text().unwrap(), "first");
    }
}
