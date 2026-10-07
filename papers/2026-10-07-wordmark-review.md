# Startup wordmark and README art: one-round review

Reviewed the six-row wordmark, startup geometry, changed direct checks and README
asset placement. No actionable defect found in this diff.

The font is 67 terminal cells wide on every row: an independent libc `wcwidth`
inspection under `en_US.UTF-8` found all its scalars single-cell. Its compile-time
scalar-count assertion is appropriate for this fixed glyph set; runtime image
layout uses the renderer's display-width rules rather than UTF-8 byte length.
Narrow or short viewports select the plain label without slicing Unicode glyphs.

At 80x24, the wordmark occupies rows 2–7 and columns 2–68; the image occupies rows
9–19 and columns 2–23, before the composer begins at row 20. At 120x40, the image
occupies rows 2–24 and columns 2–47, while the wordmark occupies rows 10–15 and
columns 52–118. Coordinates are zero-based. The side-by-side gate reserves the
wordmark and gap before computing image size; the stacked branch subtracts the
wordmark and blank row before computing image height. Small-frame handling still
erases image placement below the existing terminal-size threshold.

The aircraft's upload, placement cache, cursor save/restore, serialized output
buffer and named-image cleanup are unchanged. The PTY changes retain checks for
one upload, movement to the square avatar, cursor preservation and cleanup.
The owner reports the release build and focused chat-view, rendering-oracle and
terminal-PTY checks passing; this review did not repeat them.

The README image is separate from the harness asset. Its SHA-256 matches the
operator attachment exactly:
`6184d61caffc0d8ca44dee3f28335925b9fb90f9c7338cc02ae8a73ba569d0e3`.
The relative image path resolves from the repository README, and its alt text
provides a textual description.
