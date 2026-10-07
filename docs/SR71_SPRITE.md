# Blackbird's SR-71

The small header mascot is an original 36x8 one-bit silhouette of Kelly Johnson's
SR-71 banking in a three-quarter view, with swept wings and separated twin
nacelles. It is encoded as18x2 Braille cells by src/sprite.cpp. It uses the existing two
header rows, reserves room for title/status, and disappears below90columns.
When work is active the existing animation clock changes its exhaust. At idle
there is no mascot timer or redraw. Four cached UTF-8 frames, no image decoder,
graphics protocol, asset download or new library. `BLACKBIRD_MASCOT=0` disables it.

No Caves of Qud sprite or derived asset is used. Historical removed-mascot research
remains historical; this code-drawn aircraft carries no borrowed asset attribution.
