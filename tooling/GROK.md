# Grok Build colleague tooling

Installed2026-10-07 from the official https://x.ai/cli/install.sh installer, downloaded
and inspected before execution. Stable native macOS Apple Silicon release:
`grok 1.0.46 (2765805b9442)`. Executable ~/.grok/downloads/grok-macos-aarch64,
~/.grok/bin/grok and ~/.local/bin/grok symlink. Installer also exposes agent alias,
adds PATH/completions to ~/.zshrc and records [cli] installer in ~/.grok/config.toml.
Native binary143MiB, shared external tooling, not bundled with Arconaut.
SHA256 e8daa302364c9c3b6a5546d511cfbd1ab5e5d407a9b04282f660665ea405f9f3.

Version/help/login-help executed successfully. No existing stored Grok auth or
XAI_API_KEY/GROK_DEPLOYMENT_KEY in inherited environment. Inference untested.
Operator browser sign-in:

```sh
grok login
```

Installed CLI also supports `grok login --device-auth`, explicit model listing
`grok models`, headless `-p`/`--prompt-file`, `--output-format json` or streaming-json,
`--tools`, `--no-subagents`, `--max-turns`, `--system-prompt-override`, ACP via agent
subcommand. Consult actual installed help before composing bounded colleague calls;
source-defined tool/context/lifetime behavior needs study, not assumed from flags.
Existing native arco-colleague slice supports OpenAI/Claude only; Grok transport
not implemented or claimed by installing this external CLI. No project dependency
adoption, Kimi resubscription or inference purchase.

Primary docs: https://docs.x.ai/build/overview and
https://docs.x.ai/build/cli/reference; source https://github.com/xai-org/grok-build.
Installer/raw captures ignored in context/grok-install/. Credentials remain private.

Operator authenticated immediately after installation. Actual bounded no-tool,
one-turn readiness completed2.768s with READY, modelUsage grok-4.7-build. Reported
usage input14606/cache-read1152/output53/reasoning52/total15811; CLI overhead remains
present despite minimal selected context. Monetary field is provider-reported, not
independent subscription billing. A separate CRC source review reached60s outer bound,
exit143/no output; remote result unknown, not a completed review. Captures in ignored
context/grok-install/{readiness,crc-review}.json. No native Grok adapter delivered.
