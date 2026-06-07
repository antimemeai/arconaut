//! Ghostty terminal optimizations.
//!
//! Implements:
//! - Kitty Keyboard Protocol (push/pop enhancement flags)
//! - Synchronized output (bracket frame draws)
//! - OSC 133 semantic prompt zones
//! - Mode 2031 theme query

use std::io::{self, Write};

use crossterm::{
    event::{
        KeyboardEnhancementFlags, PopKeyboardEnhancementFlags,
        PushKeyboardEnhancementFlags,
    },
    execute, queue,
    terminal::{BeginSynchronizedUpdate, EndSynchronizedUpdate},
};

/// Push kitty keyboard enhancement flags on startup.
pub fn push_keyboard_flags() -> io::Result<()> {
    let flags = KeyboardEnhancementFlags::DISAMBIGUATE_ESCAPE_CODES
        | KeyboardEnhancementFlags::REPORT_EVENT_TYPES
        | KeyboardEnhancementFlags::REPORT_ALTERNATE_KEYS
        | KeyboardEnhancementFlags::REPORT_ALL_KEYS_AS_ESCAPE_CODES;
    execute!(io::stdout(), PushKeyboardEnhancementFlags(flags))
}

/// Pop kitty keyboard enhancement flags on shutdown.
pub fn pop_keyboard_flags() -> io::Result<()> {
    execute!(io::stdout(), PopKeyboardEnhancementFlags)
}

/// Begin a synchronized update before drawing a frame.
pub fn begin_sync() -> io::Result<()> {
    queue!(io::stdout(), BeginSynchronizedUpdate)
}

/// End a synchronized update after drawing a frame.
pub fn end_sync() -> io::Result<()> {
    queue!(io::stdout(), EndSynchronizedUpdate)
}

/// Emit OSC 133 prompt-start sequence.
pub fn osc133_prompt() {
    let _ = io::stdout().write_all(b"\x1b]133;A\x07");
}

/// Emit OSC 133 input-start sequence.
pub fn osc133_input() {
    let _ = io::stdout().write_all(b"\x1b]133;B\x07");
}

/// Emit OSC 133 output-start sequence.
pub fn osc133_output() {
    let _ = io::stdout().write_all(b"\x1b]133;C\x07");
}

/// Emit OSC 133 output-end sequence.
pub fn osc133_end_output() {
    let _ = io::stdout().write_all(b"\x1b]133;D\x07");
}

/// Query terminal for dark/light theme via mode 2031 (OSC 996).
pub fn query_theme() {
    let _ = io::stdout().write_all(b"\x1b]996\x07");
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn keyboard_flags_push_pop() {
        // These should not panic even if terminal doesn't support them.
        let _ = push_keyboard_flags();
        let _ = pop_keyboard_flags();
    }

    #[test]
    fn sync_output_queue() {
        let _ = begin_sync();
        let _ = end_sync();
    }

    #[test]
    fn osc133_sequences_no_panic() {
        osc133_prompt();
        osc133_input();
        osc133_output();
        osc133_end_output();
    }

    #[test]
    fn mode_2031_query_no_panic() {
        query_theme();
    }
}
