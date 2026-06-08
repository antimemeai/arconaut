//! Minimal wiring module for P5.7 Phase A.
//!
//! Behind the `p5_7` feature flag, this module demonstrates how to wire
//! arconaut-pilot (Soul v2) with arconaut-reactor (ArconautLogic).

use arconaut_machine::ChatProvider;
use arconaut_pilot::{DefaultSoul, SoulConfig};
use arconaut_reactor::{ArconautLogic, ReactorConfig};

/// Construct a `DefaultSoul` from an existing provider and run a minimal turn
/// through `ArconautLogic`.
pub async fn run_minimal_turn(provider: Box<dyn ChatProvider>, user_input: &str) {
    let mut soul = DefaultSoul::new(provider, SoulConfig::default());
    let mut logic = ArconautLogic::new(ReactorConfig::default());
    let _ = logic.run_turn(&mut soul, user_input).await;
}
