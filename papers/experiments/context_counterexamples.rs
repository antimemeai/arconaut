use arconaut_core::{Context,Message,ContentPart,ToolResult};
#[path = "../../quarantine/arconaut/crates/arconaut-agent/src/compaction.rs"]
mod compaction;
fn main() {
    let payload = "a".repeat(400);
    let mut direct = Context::new(1000);
    direct.append_message(Message::user(payload.clone()));
    let mut nested = Context::new(1000);
    nested.append_message(Message::tool_result("call-1", ToolResult::success(vec![ContentPart::text(payload)])));
    println!("same_400_ascii_text user_tokens={} tool_result_tokens={}", direct.token_count(), nested.token_count());
    assert_eq!(direct.token_count(),100);
    assert_eq!(nested.token_count(),0);
    let checkpoint=direct.checkpoint();
    direct.clear();
    direct.revert_to(checkpoint).unwrap();
    println!("revert_after_clear history_len={} token_count={}", direct.history().len(),direct.token_count());
    assert_eq!(direct.history().len(),0);
    assert_eq!(direct.token_count(),100);
    let engine=compaction::CompactionEngine::new().with_threshold(0.5).with_preserve_window(3);
    let mut inherited_sample=Context::new(100);
    for i in 0..10 { inherited_sample.append_message(Message::user(format!("msg{i}"))); }
    let did_compact=engine.compact(&mut inherited_sample);
    println!("inherited_preserves_recent_fixture compacted={} history_len={} tokens={}",did_compact,inherited_sample.history().len(),inherited_sample.token_count());
    assert!(!did_compact);
}
