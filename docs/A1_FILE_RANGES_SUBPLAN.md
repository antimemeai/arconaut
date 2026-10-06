# A1 — file read presentation ranges

Keep the existing 16 MiB full-read capacity and audit path. LocalTools reads and
captures the exact full file once; only the returned presentation is sliced.
This follows the separate-original/view lesson in capabilities GPTMe and SYNTHESIS,
not any imported implementation. CodingEngine's observer retains file.read before
returning the model result.

Optional byte_start/byte_end use zero-based half-open offsets. Optional
line_start/line_end use one-based inclusive lines, retaining LF and CR bytes.
A final LF does not create a phantom line; empty files have zero lines. Missing
start defaults to the first byte/line; missing end defaults to EOF. Byte and line
fields cannot mix. Negative, nonintegral, reversed ranges and line zero fail.
Valid starts beyond EOF return empty; ends beyond EOF clip. Full reads preserve
the existing content-only result. Binary presentation uses existing hex fallback.

Direct tools tests cover endpoints, empty files, LF/CRLF/unterminated lines,
binary slicing, audit originals, invalid numeric types and overflow. Build release
arco and affected tests, restart, and verify ranges through the resumed Lua host.
A1/A2 batch review and sanitizer/portable checks follow A2.
