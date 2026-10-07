# Restore acquired multiplayer references

Run from the Arconaut repository root. Archives remain intact under the shared quarantine directory. Restore from listed immutable URLs, check SHA-256, then use the safe ingester. Destinations must be new. No upstream code is run.

## ValveSoftware/GameNetworkingSockets

    curl --fail --location https://codeload.github.com/ValveSoftware/GameNetworkingSockets/zip/d534c19aa760df3fb75fd20db13ba1932b8a5463 --output ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/GameNetworkingSockets-d534c19aa760df3fb75fd20db13ba1932b8a5463.zip
    shasum -a 256 ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/GameNetworkingSockets-d534c19aa760df3fb75fd20db13ba1932b8a5463.zip
    # Expected SHA-256: 34475423f4d03494b9b8b4c0703b2807ea2fcf7035cbb1da44e0ed3a0ff7d48b
    python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/GameNetworkingSockets-d534c19aa760df3fb75fd20db13ba1932b8a5463.zip quarantine/multiplayer-study/GameNetworkingSockets --root GameNetworkingSockets-d534c19aa760df3fb75fd20db13ba1932b8a5463 --skip-symlinks

## lsalzman/enet

    curl --fail --location https://codeload.github.com/lsalzman/enet/zip/5a9c537fd464b3c6d3c55e1d3bd47588faf71b42 --output ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/enet-5a9c537fd464b3c6d3c55e1d3bd47588faf71b42.zip
    shasum -a 256 ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/enet-5a9c537fd464b3c6d3c55e1d3bd47588faf71b42.zip
    # Expected SHA-256: f01da8ea686147d04bb8524d580dc26b44015b9b7f0ddde012a357e2e3a35fde
    python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/enet-5a9c537fd464b3c6d3c55e1d3bd47588faf71b42.zip quarantine/multiplayer-study/enet --root enet-5a9c537fd464b3c6d3c55e1d3bd47588faf71b42 --skip-symlinks

## pond3r/ggpo

    curl --fail --location https://codeload.github.com/pond3r/ggpo/zip/7ddadef8546a7d99ff0b3530c6056bc8ee4b9c0a --output ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/ggpo-7ddadef8546a7d99ff0b3530c6056bc8ee4b9c0a.zip
    shasum -a 256 ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/ggpo-7ddadef8546a7d99ff0b3530c6056bc8ee4b9c0a.zip
    # Expected SHA-256: 6d49623b2db2a342f4a32255b868ebb2c05bdbdd72d5cf66a3bf08488e9d3ba3
    python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/ggpo-7ddadef8546a7d99ff0b3530c6056bc8ee4b9c0a.zip quarantine/multiplayer-study/ggpo --root ggpo-7ddadef8546a7d99ff0b3530c6056bc8ee4b9c0a --skip-symlinks

## automerge/automerge

    curl --fail --location https://codeload.github.com/automerge/automerge/zip/e2452ea1ea9a4008e94f435d60e893bfeb8e0f38 --output ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/automerge-e2452ea1ea9a4008e94f435d60e893bfeb8e0f38.zip
    shasum -a 256 ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/automerge-e2452ea1ea9a4008e94f435d60e893bfeb8e0f38.zip
    # Expected SHA-256: 7a542b0a1b06323f0cf603d6cc08725173f21f25311928e275d5355cece0b7be
    python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-multiplayer-2026-10-07/automerge-e2452ea1ea9a4008e94f435d60e893bfeb8e0f38.zip quarantine/multiplayer-study/automerge --root automerge-e2452ea1ea9a4008e94f435d60e893bfeb8e0f38 --skip-symlinks

