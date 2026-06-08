use std::path::PathBuf;
use std::time::Duration;

/// Beat state injected into context.
#[derive(Debug, Clone, Default)]
pub struct BeatState {
    pub base_dir: PathBuf,
    pub work_dir: PathBuf,
    pub session_elapsed: Duration,
    pub active_phase: String,
    pub last_tool: Option<String>,
    pub last_file: Option<PathBuf>,
    pub compactions: usize,
    pub context_used: (usize, usize), // (used, total)
    pub working_set: Vec<PathBuf>,
    pub last_test_run: Option<String>,
}
