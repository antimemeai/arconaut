# Startup and square status sprite: one bounded adversarial review

Reviewed the native uncommitted sprite API, cached Braille encoder, ASCII startup composition, terminal welcome lifecycle/right gutter, main plain-terminal startup, and the companion board's square avatar/header changes on 2026-10-07. This is the single requested implementation review; no rereview is implied.

## Concrete findings

1. **Short-height avatar collision.** Native `show_sprite` initially checked terminal width only. With an 8-row, sufficiently wide terminal and a normal one-row draft, transcript height is two: the six-row avatar writes rows 0–5, then the menu/composer starts at row 4 and overwrites its bottom. A large command palette can produce the same collision on taller terminals. Gate the avatar on `height + 2 >= blackbird_sprite_height`, so the entire square fits above the menu; do not reserve a chat gutter when hidden.

2. **Startup lifecycle differs on restored sessions.** Any restored user/assistant/text/process message dismisses `welcoming` before first paint. A normal session with retained history consequently skips the large aircraft/lettering, while a fresh session displays it. The operator requested large art on startup, then a small avatar. Either show the startup view before restored history takes over or make the fresh-only behavior an explicit product decision. This is a behavior mismatch, not memory corruption.

## Other inspected properties

Native frame references have static lifetime; their four cached encodings bound active phase indexing. Braille lookup coordinates stay within the 24×24 bitmap. Exhaust coordinates stay in range. The gutter narrows transcript wrapping before overlay, keeping content out of the avatar columns. State color precedence makes failed turns red, tools amber, model work cyan, and idle muted; starting a turn resets failure/tool flags. Animation shares the existing busy clock and adds no idle wake.

ASCII startup composition uses fixed local strings and does not enter provider requests or retained audit. Board avatar coordinates are bounded, header text reserves its right gutter, and the frame overlay occurs after header text/rule output. The fleet-card sprite was replaced by a single-line aircraft label, preserving card status rows. No new concrete board bounds/lifetime defect found in this diff.

The parent owns final focused checks and disposition. No production files edited by this reviewer.
