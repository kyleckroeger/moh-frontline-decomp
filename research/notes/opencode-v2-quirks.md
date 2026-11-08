# opencode-v2-quirks

Agent working notes, kept as written; dated claims may be out of date.


Verified on 2026-10-04 by live tests. opencode v2.0.22 is installed at ~/.opencode/bin/opencode; it is not on PATH in Claude's shell.

- **Config loading:** OPENCODE_CONFIG and OPENCODE_CONFIG_DIR are IGNORED. Config is read only from the run directory (`opencode.json` or `.opencode/opencode.json`) and from ~/.config/opencode. `opencode debug config` lists the sources it found.
- **`opencode debug agents`:** does not show project permission rules. Test permissions with a live `run` instead.
- **`run` flags:** there is no `--dir`; it runs in the cwd. Use `--standalone` to get a private server, because the default background service may not pick up the project config. `--format json` events carry `"sessionID"`, and `-s` resumes a session.
- **The user's model:** `opencode-go/deepseek-v4.1-flash#high`, through the OpenCode Console account. The "Go" provider; there is also `opencode/…`.
- **Bash allowlist:** `;`, `&&` and `$(…)` were already blocked. `>` redirects were NOT blocked until explicit deny rules for `*>*`, `*|*` and similar were added after the allow rules (last match wins).
- **Cost:** a three-question plan-mode session cost about $0.03.

Related: [[agent-pipeline]]
