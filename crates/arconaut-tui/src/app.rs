use std::time::Duration;

use crossterm::event::{self, Event, KeyCode, KeyEvent, KeyModifiers, MouseEventKind};
use ratatui::layout::{Constraint, Direction, Layout};
use ratatui::Frame;
use tokio::sync::mpsc;

use crate::protocol::{SoulCommand, TuiEvent};
use crate::widgets::{render_chat_pane, render_header, render_input, render_status_bar, render_terminal_pane};

/// Which UI region has focus.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Focus {
    Chat,
    Terminal,
    Input,
}

/// A single item in the chat transcript.
#[derive(Debug, Clone)]
pub enum ChatItem {
    User(String),
    Assistant(String),
    ToolCall { name: String, args: String },
    ToolResult { name: String, result: String },
    Error(String),
    Status(String),
}

/// Application state and event loop for the Arconaut TUI.
pub struct App {
    pub messages: Vec<ChatItem>,
    pub terminal_parser: vt100::Parser,
    pub input_buffer: String,
    pub input_cursor: usize,
    pub focus: Focus,
    pub split_ratio: f64,
    pub status: String,
    pub thinking: Option<String>,
    pub running: bool,
    pub scroll_offset: usize,
    pub terminal_scroll: usize,
    pub soul_tx: mpsc::Sender<SoulCommand>,
    pub tui_rx: mpsc::Receiver<TuiEvent>,
}

impl App {
    pub fn new(
        soul_tx: mpsc::Sender<SoulCommand>,
        tui_rx: mpsc::Receiver<TuiEvent>,
        term_rows: u16,
        term_cols: u16,
    ) -> Self {
        Self {
            messages: Vec::new(),
            terminal_parser: vt100::Parser::new(term_rows, term_cols, 1000),
            input_buffer: String::new(),
            input_cursor: 0,
            focus: Focus::Input,
            split_ratio: 0.5,
            status: String::from("Ready"),
            thinking: None,
            running: true,
            scroll_offset: 0,
            terminal_scroll: 0,
            soul_tx,
            tui_rx,
        }
    }

    /// Run the TUI event loop until `running` becomes false.
    pub async fn run<B: ratatui::backend::Backend>(
        &mut self,
        terminal: &mut ratatui::Terminal<B>,
    ) -> std::io::Result<()>
    where
        B::Error: std::fmt::Debug,
    {
        // Spawn a blocking task to read crossterm events.
        let (key_tx, mut key_rx) = mpsc::channel::<Event>(100);
        std::thread::spawn(move || {
            loop {
                if event::poll(Duration::from_millis(50)).unwrap_or(false) {
                    if let Ok(ev) = event::read() {
                        if key_tx.blocking_send(ev).is_err() {
                            break;
                        }
                    }
                }
            }
        });

        while self.running {
            crate::ghostty::osc133_prompt();
            tokio::select! {
                Some(ev) = key_rx.recv() => {
                    self.handle_crossterm_event(ev).await;
                }
                Some(ev) = self.tui_rx.recv() => {
                    self.handle_tui_event(ev);
                }
                else => {
                    // Both channels closed; exit.
                    self.running = false;
                }
            }

            crate::ghostty::begin_sync()?;
            terminal
                .draw(|f| self.draw(f))
                .map_err(|e| std::io::Error::other(format!("{:?}", e)))?;
            crate::ghostty::end_sync()?;
        }

        Ok(())
    }

    async fn handle_crossterm_event(&mut self, ev: Event) {
        match ev {
            Event::Key(key) => self.handle_key(key).await,
            Event::Mouse(mouse) => self.handle_mouse(mouse).await,
            Event::Resize(cols, rows) => {
                let _ = self.soul_tx.send(SoulCommand::Resize(rows, cols)).await;
                self.terminal_parser.screen_mut().set_size(rows, cols);
            }
            _ => {}
        }
    }

    async fn handle_key(&mut self, key: KeyEvent) {
        match key.code {
            KeyCode::Char('c') if key.modifiers.contains(KeyModifiers::CONTROL) => {
                let _ = self.soul_tx.send(SoulCommand::Quit).await;
                self.running = false;
            }
            KeyCode::Char('d') if key.modifiers.contains(KeyModifiers::CONTROL) => {
                let _ = self.soul_tx.send(SoulCommand::Quit).await;
                self.running = false;
            }
            KeyCode::Char('c') if key.modifiers.contains(KeyModifiers::ALT) => {
                let _ = self.soul_tx.send(SoulCommand::Interrupt).await;
                self.status = "Interrupted".to_string();
            }
            KeyCode::Tab => {
                self.focus = match self.focus {
                    Focus::Chat => Focus::Terminal,
                    Focus::Terminal => Focus::Input,
                    Focus::Input => Focus::Chat,
                };
            }
            KeyCode::BackTab => {
                self.focus = match self.focus {
                    Focus::Chat => Focus::Input,
                    Focus::Terminal => Focus::Chat,
                    Focus::Input => Focus::Terminal,
                };
            }
            KeyCode::Left if key.modifiers.contains(KeyModifiers::ALT) => {
                self.split_ratio = (self.split_ratio - 0.05).clamp(0.1, 0.9);
            }
            KeyCode::Right if key.modifiers.contains(KeyModifiers::ALT) => {
                self.split_ratio = (self.split_ratio + 0.05).clamp(0.1, 0.9);
            }
            KeyCode::Up => {
                match self.focus {
                    Focus::Chat => self.scroll_offset = self.scroll_offset.saturating_add(1),
                    Focus::Terminal => self.terminal_scroll = self.terminal_scroll.saturating_add(1),
                    Focus::Input => {}
                }
            }
            KeyCode::Down => {
                match self.focus {
                    Focus::Chat => self.scroll_offset = self.scroll_offset.saturating_sub(1),
                    Focus::Terminal => self.terminal_scroll = self.terminal_scroll.saturating_sub(1),
                    Focus::Input => {}
                }
            }
            KeyCode::Enter => {
                if self.focus == Focus::Input && !self.input_buffer.is_empty() {
                    let text = self.input_buffer.clone();
                    self.input_buffer.clear();
                    self.input_cursor = 0;
                    self.messages.push(ChatItem::User(text.clone()));
                    let _ = self.soul_tx.send(SoulCommand::UserInput(text)).await;
                } else if self.focus == Focus::Terminal {
                    let _ = self
                        .soul_tx
                        .send(SoulCommand::TerminalOutput(self.input_buffer.clone()))
                        .await;
                    self.input_buffer.clear();
                    self.input_cursor = 0;
                }
            }
            KeyCode::Char(c) => {
                if self.focus == Focus::Input || self.focus == Focus::Terminal {
                    self.input_buffer.insert(self.input_cursor, c);
                    self.input_cursor += 1;
                }
            }
            KeyCode::Backspace => {
                if (self.focus == Focus::Input || self.focus == Focus::Terminal)
                    && self.input_cursor > 0
                {
                    self.input_cursor -= 1;
                    self.input_buffer.remove(self.input_cursor);
                }
            }
            KeyCode::Delete => {
                if (self.focus == Focus::Input || self.focus == Focus::Terminal)
                    && self.input_cursor < self.input_buffer.len()
                {
                    self.input_buffer.remove(self.input_cursor);
                }
            }
            KeyCode::Left => {
                if self.focus == Focus::Input || self.focus == Focus::Terminal {
                    self.input_cursor = self.input_cursor.saturating_sub(1);
                }
            }
            KeyCode::Right => {
                if (self.focus == Focus::Input || self.focus == Focus::Terminal)
                    && self.input_cursor < self.input_buffer.len()
                {
                    self.input_cursor += 1;
                }
            }
            KeyCode::Home => {
                self.input_cursor = 0;
            }
            KeyCode::End => {
                self.input_cursor = self.input_buffer.len();
            }
            _ => {}
        }
    }

    async fn handle_mouse(&mut self, mouse: crossterm::event::MouseEvent) {
        if mouse.kind == MouseEventKind::ScrollDown {
            match self.focus {
                Focus::Chat => self.scroll_offset = self.scroll_offset.saturating_sub(3),
                Focus::Terminal => self.terminal_scroll = self.terminal_scroll.saturating_sub(3),
                Focus::Input => {}
            }
        } else if mouse.kind == MouseEventKind::ScrollUp {
            match self.focus {
                Focus::Chat => self.scroll_offset = self.scroll_offset.saturating_add(3),
                Focus::Terminal => self.terminal_scroll = self.terminal_scroll.saturating_add(3),
                Focus::Input => {}
            }
        }
        // TODO: draggable pane divider
    }

    fn handle_tui_event(&mut self, ev: TuiEvent) {
        match ev {
            TuiEvent::AssistantMessage(msg) => {
                crate::ghostty::osc133_output();
                let text: String = msg
                    .content
                    .iter()
                    .filter_map(|part| part.as_text())
                    .collect::<Vec<_>>()
                    .join("");
                if !text.is_empty() {
                    self.messages.push(ChatItem::Assistant(text));
                }
            }
            TuiEvent::ToolCall { name, args } => {
                self.messages.push(ChatItem::ToolCall {
                    name,
                    args: args.to_string(),
                });
            }
            TuiEvent::ToolResult { name, result } => {
                self.messages.push(ChatItem::ToolResult {
                    name,
                    result: format!("{:?}", result),
                });
            }
            TuiEvent::Thinking { text } => {
                self.thinking = Some(text);
            }
            TuiEvent::Error { message } => {
                self.messages.push(ChatItem::Error(message));
            }
            TuiEvent::ShellOutput(text) => {
                self.terminal_parser.process(text.as_bytes());
            }
            TuiEvent::Status(text) => {
                self.status = text;
            }
            TuiEvent::TurnComplete => {
                crate::ghostty::osc133_end_output();
                self.status = "Ready".to_string();
                self.thinking = None;
            }
        }
    }

    pub fn draw(&mut self, frame: &mut Frame) {
        let area = frame.area();

        let chunks = Layout::default()
            .direction(Direction::Vertical)
            .constraints([
                Constraint::Length(1), // header
                Constraint::Min(1),    // main content
                Constraint::Length(3), // input
                Constraint::Length(1), // status bar
            ])
            .split(area);

        let header_area = chunks[0];
        let main_area = chunks[1];
        let input_area = chunks[2];
        let status_area = chunks[3];

        // Split main area horizontally for chat + terminal.
        let split = (self.split_ratio * main_area.width as f64) as u16;
        let main_chunks = Layout::default()
            .direction(Direction::Horizontal)
            .constraints([Constraint::Length(split), Constraint::Min(1)])
            .split(main_area);

        let chat_area = main_chunks[0];
        let term_area = main_chunks[1];

        render_header(frame, header_area);
        render_chat_pane(frame, chat_area, &self.messages, self.focus == Focus::Chat, self.scroll_offset);
        render_terminal_pane(frame, term_area, self.terminal_parser.screen(), self.focus == Focus::Terminal, self.terminal_scroll);
        render_input(frame, input_area, &self.input_buffer, self.input_cursor, self.focus == Focus::Input);
        render_status_bar(frame, status_area, &self.status, self.focus);
    }
}
