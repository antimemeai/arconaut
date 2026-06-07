use std::collections::HashMap;
use tokio::sync::{broadcast, RwLock};

/// A message on the agent bus.
#[derive(Debug, Clone, PartialEq)]
pub struct BusMessage {
    pub from: String,
    pub to: Target,
    pub body: String,
}

/// Who a message is addressed to.
#[derive(Debug, Clone, PartialEq)]
pub enum Target {
    /// Broadcast to a topic/channel (e.g. "#general").
    Topic(String),
    /// Direct message to a specific agent.
    Agent(String),
}

/// Presence info for an agent on the bus.
#[derive(Debug, Clone, Default)]
pub struct Presence {
    pub online: bool,
    pub current_task: Option<String>,
}

/// SLLMack — the agent-to-agent messaging bus.
///
/// Lightweight pub/sub over tokio broadcast channels.  Agents join topics,
/// receive broadcasts, and can whisper each other directly.
pub struct Bus {
    /// Topic name → broadcast channel for that topic.
    topics: RwLock<HashMap<String, broadcast::Sender<BusMessage>>>,
    /// Direct-message mailboxes: agent name → sender half.
    mailboxes: RwLock<HashMap<String, broadcast::Sender<BusMessage>>>,
    /// Presence table.
    presence: RwLock<HashMap<String, Presence>>,
}

impl Bus {
    pub fn new() -> Self {
        Self {
            topics: RwLock::new(HashMap::new()),
            mailboxes: RwLock::new(HashMap::new()),
            presence: RwLock::new(HashMap::new()),
        }
    }

    /// Join a topic, returning a receiver for broadcasts on that topic.
    /// Creates the topic if it does not exist.
    pub async fn join_topic(&self, topic: impl Into<String>) -> broadcast::Receiver<BusMessage> {
        let topic = topic.into();
        let mut topics = self.topics.write().await;
        let tx = topics
            .entry(topic)
            .or_insert_with(|| broadcast::channel(256).0);
        tx.subscribe()
    }

    /// Leave a topic (drop the receiver — no-op server-side).
    pub async fn leave_topic(&self, _topic: impl Into<String>) {
        // Receiver is dropped by caller; server has no per-subscriber state.
    }

    /// Register a direct-message mailbox for an agent.
    pub async fn register_mailbox(
        &self,
        agent: impl Into<String>,
    ) -> broadcast::Receiver<BusMessage> {
        let agent = agent.into();
        let mut boxes = self.mailboxes.write().await;
        let tx = boxes
            .entry(agent.clone())
            .or_insert_with(|| broadcast::channel(256).0);
        tx.subscribe()
    }

    /// Unregister a mailbox.
    pub async fn unregister_mailbox(&self, agent: impl Into<String>) {
        let mut boxes = self.mailboxes.write().await;
        boxes.remove(&agent.into());
    }

    /// Broadcast a message to a topic.
    pub async fn broadcast(&self, topic: impl Into<String>, msg: BusMessage) {
        let topic = topic.into();
        let topics = self.topics.read().await;
        if let Some(tx) = topics.get(&topic) {
            let _ = tx.send(msg);
        }
    }

    /// Send a direct message to an agent.
    pub async fn whisper(&self, to: impl Into<String>, msg: BusMessage) {
        let to = to.into();
        let boxes = self.mailboxes.read().await;
        if let Some(tx) = boxes.get(&to) {
            let _ = tx.send(msg);
        }
    }

    /// Set presence for an agent.
    pub async fn set_presence(&self, agent: impl Into<String>, presence: Presence) {
        let mut p = self.presence.write().await;
        p.insert(agent.into(), presence);
    }

    /// Get presence for an agent.
    pub async fn get_presence(&self, agent: &str) -> Option<Presence> {
        let p = self.presence.read().await;
        p.get(agent).cloned()
    }

    /// List all agents with presence info.
    pub async fn list_presence(&self) -> Vec<(String, Presence)> {
        let p = self.presence.read().await;
        p.iter().map(|(k, v)| (k.clone(), v.clone())).collect()
    }
}

impl Default for Bus {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn broadcast_reaches_subscribers() {
        let bus = Bus::new();
        let mut rx = bus.join_topic("#general").await;

        bus.broadcast(
            "#general",
            BusMessage {
                from: "alpha".to_string(),
                to: Target::Topic("#general".to_string()),
                body: "hello all".to_string(),
            },
        )
        .await;

        let msg = rx.recv().await.unwrap();
        assert_eq!(msg.from, "alpha");
        assert_eq!(msg.body, "hello all");
    }

    #[tokio::test]
    async fn whisper_reaches_mailbox() {
        let bus = Bus::new();
        let mut rx = bus.register_mailbox("beta").await;

        bus.whisper(
            "beta",
            BusMessage {
                from: "alpha".to_string(),
                to: Target::Agent("beta".to_string()),
                body: "psst".to_string(),
            },
        )
        .await;

        let msg = rx.recv().await.unwrap();
        assert_eq!(msg.body, "psst");
    }

    #[tokio::test]
    async fn presence_tracked() {
        let bus = Bus::new();
        bus.set_presence(
            "gamma",
            Presence {
                online: true,
                current_task: Some("refactor".to_string()),
            },
        )
        .await;

        let p = bus.get_presence("gamma").await.unwrap();
        assert!(p.online);
        assert_eq!(p.current_task, Some("refactor".to_string()));
    }

    #[tokio::test]
    async fn no_presence_for_unknown() {
        let bus = Bus::new();
        assert!(bus.get_presence("nobody").await.is_none());
    }

    #[tokio::test]
    async fn list_presence() {
        let bus = Bus::new();
        bus.set_presence("a", Presence::default()).await;
        bus.set_presence("b", Presence::default()).await;
        let list = bus.list_presence().await;
        assert_eq!(list.len(), 2);
    }
}
