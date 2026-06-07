use std::collections::HashMap;
use std::pin::Pin;
use std::sync::{Arc, Mutex};
use tokio::sync::{mpsc, RwLock};
use tokio_stream::{wrappers::ReceiverStream, Stream};
use tonic::{Request, Response, Status};

use crate::inbox::agent_inbox_server::{AgentInbox, AgentInboxServer};
use crate::inbox::{AgentId, AgentStatus, Empty, Message, StreamRequest};

/// In-memory state for the gRPC inbox.
#[derive(Debug, Default)]
struct InboxState {
    messages: Vec<Message>,
    subscribers: HashMap<String, mpsc::Sender<Message>>,
    online: HashMap<String, bool>,
    tasks: HashMap<String, String>,
}

/// gRPC inbox server for inter-agent communication.
pub struct InboxServer {
    state: Arc<RwLock<InboxState>>,
}

impl InboxServer {
    pub fn new() -> Self {
        Self {
            state: Arc::new(RwLock::new(InboxState::default())),
        }
    }

    pub fn into_service(self) -> AgentInboxServer<Self> {
        AgentInboxServer::new(self)
    }

    pub async fn send(&self, msg: Message) {
        let state = self.state.read().await;
        if let Some(tx) = state.subscribers.get(&msg.to) {
            let _ = tx.send(msg.clone()).await;
        }
    }
}

#[tonic::async_trait]
impl AgentInbox for InboxServer {
    async fn send_message(
        &self,
        request: Request<Message>,
    ) -> Result<Response<Empty>, Status> {
        let msg = request.into_inner();
        let mut state = self.state.write().await;
        state.messages.push(msg.clone());
        if let Some(tx) = state.subscribers.get(&msg.to) {
            let _ = tx.send(msg).await;
        }
        Ok(Response::new(Empty {}))
    }

    type StreamMessagesStream = Pin<Box<dyn Stream<Item = Result<Message, Status>> + Send>>;

    async fn stream_messages(
        &self,
        request: Request<StreamRequest>,
    ) -> Result<Response<Self::StreamMessagesStream>, Status> {
        let agent_name = request.into_inner().agent_name;
        let (tx, rx) = mpsc::channel(100);
        {
            let mut state = self.state.write().await;
            // Remove old subscriber for this agent to prevent channel leaks.
            state.subscribers.remove(&agent_name);
            state.subscribers.insert(agent_name, tx);
        }
        let stream = ReceiverStream::new(rx).map(Ok);
        Ok(Response::new(Box::pin(stream) as Self::StreamMessagesStream))
    }

    async fn get_status(
        &self,
        request: Request<AgentId>,
    ) -> Result<Response<AgentStatus>, Status> {
        let name = request.into_inner().name;
        let state = self.state.read().await;
        let status = AgentStatus {
            name: name.clone(),
            online: *state.online.get(&name).unwrap_or(&false),
            current_task: state.tasks.get(&name).cloned().unwrap_or_default(),
        };
        Ok(Response::new(status))
    }
}

/// Client wrapper for the gRPC inbox.
pub struct InboxClient {
    inner: crate::inbox::agent_inbox_client::AgentInboxClient<tonic::transport::Channel>,
}

impl InboxClient {
    pub async fn connect(addr: impl Into<String>) -> Result<Self, tonic::transport::Error> {
        let inner = crate::inbox::agent_inbox_client::AgentInboxClient::connect(addr.into())
            .await?;
        Ok(Self { inner })
    }

    pub async fn send_message(&mut self, msg: Message) -> Result<(), Status> {
        self.inner.send_message(msg).await?;
        Ok(())
    }

    pub async fn get_status(&mut self, name: impl Into<String>) -> Result<AgentStatus, Status> {
        let status = self
            .inner
            .get_status(AgentId {
                name: name.into(),
            })
            .await?;
        Ok(status.into_inner())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tonic::transport::Server;

    #[tokio::test]
    async fn get_status() {
        let server = InboxServer::new();
        let addr = "127.0.0.1:50051".parse().unwrap();
        let svc = server.into_service();

        tokio::spawn(async move {
            Server::builder()
                .add_service(svc)
                .serve(addr)
                .await
                .unwrap();
        });

        tokio::time::sleep(tokio::time::Duration::from_millis(100)).await;

        let mut client = InboxClient::connect("http://127.0.0.1:50051")
            .await
            .unwrap();
        let status = client.get_status("alpha").await.unwrap();
        assert_eq!(status.name, "alpha");
        assert!(!status.online);
    }
}
