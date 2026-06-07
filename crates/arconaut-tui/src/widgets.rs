use ratatui::layout::{Alignment, Rect};
use ratatui::style::{Color, Modifier, Style};
use ratatui::text::{Line, Span, Text};
use ratatui::widgets::{Block, Borders, Paragraph, Wrap};
use ratatui::Frame;

use crate::app::ChatItem;

// ---------------------------------------------------------------------------
// Caves of Qud theme
// ---------------------------------------------------------------------------

pub const BG: Color = Color::Rgb(8, 8, 15);
pub const BORDER: Color = Color::Rgb(61, 43, 31);
pub const BORDER_FOCUSED: Color = Color::Rgb(184, 115, 51);
pub const USER_TEXT: Color = Color::Rgb(78, 205, 196);
pub const ASSISTANT_TEXT: Color = Color::Rgb(247, 247, 212);
pub const ERROR_TEXT: Color = Color::Rgb(255, 107, 107);
pub const TOOL_TEXT: Color = Color::Rgb(149, 225, 211);
pub const STATUS_TEXT: Color = Color::Rgb(136, 136, 136);
pub const INPUT_TEXT: Color = Color::Rgb(238, 238, 238);
pub const HEADER_BG: Color = Color::Rgb(20, 15, 10);

// ---------------------------------------------------------------------------
// Header
// ---------------------------------------------------------------------------

pub fn render_header(frame: &mut Frame, area: Rect) {
    let text = Text::from(Line::from(vec![
        Span::styled(" ARCONAUT ", Style::default().fg(BORDER_FOCUSED).bg(HEADER_BG).add_modifier(Modifier::BOLD)),
        Span::styled(" AI coding agent ", Style::default().fg(STATUS_TEXT).bg(HEADER_BG)),
    ]));
    let paragraph = Paragraph::new(text).alignment(Alignment::Left);
    frame.render_widget(paragraph, area);
}

// ---------------------------------------------------------------------------
// Chat Pane
// ---------------------------------------------------------------------------

pub fn render_chat_pane(
    frame: &mut Frame,
    area: Rect,
    messages: &[ChatItem],
    focused: bool,
    scroll_offset: usize,
) {
    let border_color = if focused { BORDER_FOCUSED } else { BORDER };
    let block = Block::default()
        .borders(Borders::ALL)
        .border_style(Style::default().fg(border_color))
        .title(Span::styled(" Chat ", Style::default().fg(border_color)))
        .style(Style::default().bg(BG));

    let inner = block.inner(area);
    frame.render_widget(block, area);

    let mut lines: Vec<Line> = Vec::new();
    for msg in messages {
        match msg {
            ChatItem::User(text) => {
                lines.push(Line::from(vec![
                    Span::styled("> ", Style::default().fg(USER_TEXT).add_modifier(Modifier::BOLD)),
                    Span::styled(text.as_str(), Style::default().fg(USER_TEXT)),
                ]));
            }
            ChatItem::Assistant(text) => {
                lines.push(Line::from(vec![
                    Span::styled("◆ ", Style::default().fg(ASSISTANT_TEXT).add_modifier(Modifier::BOLD)),
                    Span::styled(text.as_str(), Style::default().fg(ASSISTANT_TEXT)),
                ]));
            }
            ChatItem::ToolCall { name, args } => {
                lines.push(Line::from(vec![
                    Span::styled("▸ ", Style::default().fg(TOOL_TEXT)),
                    Span::styled(format!("{} ", name), Style::default().fg(TOOL_TEXT).add_modifier(Modifier::BOLD)),
                    Span::styled(args.as_str(), Style::default().fg(TOOL_TEXT).add_modifier(Modifier::ITALIC)),
                ]));
            }
            ChatItem::ToolResult { name, result } => {
                lines.push(Line::from(vec![
                    Span::styled("◂ ", Style::default().fg(TOOL_TEXT)),
                    Span::styled(format!("{} → ", name), Style::default().fg(TOOL_TEXT).add_modifier(Modifier::BOLD)),
                    Span::styled(result.as_str(), Style::default().fg(TOOL_TEXT)),
                ]));
            }
            ChatItem::Error(text) => {
                lines.push(Line::from(vec![
                    Span::styled("! ", Style::default().fg(ERROR_TEXT).add_modifier(Modifier::BOLD)),
                    Span::styled(text.as_str(), Style::default().fg(ERROR_TEXT)),
                ]));
            }
            ChatItem::Status(text) => {
                lines.push(Line::from(vec![
                    Span::styled("… ", Style::default().fg(STATUS_TEXT)),
                    Span::styled(text.as_str(), Style::default().fg(STATUS_TEXT).add_modifier(Modifier::ITALIC)),
                ]));
            }
        }
        lines.push(Line::from(""));
    }

    let scroll = scroll_offset.clamp(0, lines.len().saturating_sub(1));
    let visible_lines: Vec<Line> = lines.into_iter().skip(scroll).take(inner.height as usize).collect();

    let paragraph = Paragraph::new(Text::from(visible_lines))
        .wrap(Wrap { trim: true })
        .style(Style::default().bg(BG));
    frame.render_widget(paragraph, inner);
}

// ---------------------------------------------------------------------------
// Terminal Pane (vt100)
// ---------------------------------------------------------------------------

fn vt100_color_to_ratatui(color: vt100::Color) -> Color {
    match color {
        vt100::Color::Default => Color::Reset,
        vt100::Color::Idx(i) => Color::Indexed(i),
        vt100::Color::Rgb(r, g, b) => Color::Rgb(r, g, b),
    }
}

pub fn render_terminal_pane(
    frame: &mut Frame,
    area: Rect,
    screen: &vt100::Screen,
    focused: bool,
    scroll_offset: usize,
) {
    let border_color = if focused { BORDER_FOCUSED } else { BORDER };
    let block = Block::default()
        .borders(Borders::ALL)
        .border_style(Style::default().fg(border_color))
        .title(Span::styled(" Terminal ", Style::default().fg(border_color)))
        .style(Style::default().bg(BG));

    let inner = block.inner(area);
    frame.render_widget(block, area);

    let (rows, cols) = screen.size();
    let max_rows = inner.height;
    let max_cols = inner.width;

    let visible_rows = rows.min(max_rows);
    let visible_cols = cols.min(max_cols);

    let mut lines: Vec<Line> = Vec::with_capacity(visible_rows as usize);

    for row in 0..visible_rows {
        let actual_row = (row as usize + scroll_offset).min(rows as usize - 1) as u16;
        let mut spans: Vec<Span> = Vec::with_capacity(visible_cols as usize);
        for col in 0..visible_cols {
            if let Some(cell) = screen.cell(actual_row, col) {
                let mut fg = vt100_color_to_ratatui(cell.fgcolor());
                let mut bg = vt100_color_to_ratatui(cell.bgcolor());
                if cell.inverse() {
                    std::mem::swap(&mut fg, &mut bg);
                }
                let mut modifiers = Modifier::empty();
                if cell.bold() {
                    modifiers |= Modifier::BOLD;
                }
                if cell.italic() {
                    modifiers |= Modifier::ITALIC;
                }
                if cell.underline() {
                    modifiers |= Modifier::UNDERLINED;
                }
                if cell.dim() {
                    modifiers |= Modifier::DIM;
                }
                let style = Style::default().fg(fg).bg(bg).add_modifier(modifiers);
                spans.push(Span::styled(cell.contents(), style));
            } else {
                spans.push(Span::raw(" "));
            }
        }
        lines.push(Line::from(spans));
    }

    let paragraph = Paragraph::new(Text::from(lines)).style(Style::default().bg(BG));
    frame.render_widget(paragraph, inner);
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------

pub fn render_input(
    frame: &mut Frame,
    area: Rect,
    buffer: &str,
    cursor: usize,
    focused: bool,
) {
    let border_color = if focused { BORDER_FOCUSED } else { BORDER };
    let block = Block::default()
        .borders(Borders::ALL)
        .border_style(Style::default().fg(border_color))
        .title(Span::styled(" Input ", Style::default().fg(border_color)))
        .style(Style::default().bg(BG));

    let inner = block.inner(area);
    frame.render_widget(block, area);

    let cursor = cursor.clamp(0, buffer.len());
    let paragraph = Paragraph::new(buffer)
        .style(Style::default().fg(INPUT_TEXT).bg(BG))
        .wrap(Wrap { trim: false });
    frame.render_widget(paragraph, inner);

    if focused {
        // Simple cursor positioning: assume single-line input for now.
        let cursor_x = inner.x + (cursor as u16).min(inner.width.saturating_sub(1));
        let cursor_y = inner.y;
        frame.set_cursor_position((cursor_x, cursor_y));
    }
}

// ---------------------------------------------------------------------------
// Status Bar
// ---------------------------------------------------------------------------

pub fn render_status_bar(
    frame: &mut Frame,
    area: Rect,
    status: &str,
    focus: crate::app::Focus,
) {
    let focus_label = match focus {
        crate::app::Focus::Chat => "CHAT",
        crate::app::Focus::Terminal => "TERM",
        crate::app::Focus::Input => "INPUT",
    };
    let text = Text::from(Line::from(vec![
        Span::styled(
            format!(" [{}] ", focus_label),
            Style::default().fg(BORDER_FOCUSED).bg(HEADER_BG).add_modifier(Modifier::BOLD),
        ),
        Span::styled(
            format!(" {} ", status),
            Style::default().fg(STATUS_TEXT).bg(HEADER_BG),
        ),
    ]));
    let paragraph = Paragraph::new(text).style(Style::default().bg(HEADER_BG));
    frame.render_widget(paragraph, area);
}
