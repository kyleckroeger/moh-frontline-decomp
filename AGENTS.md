# Agent instructions

Read README.md and CONTRIBUTING.md before changing this project. AI-assisted contributions are allowed.

Keep the original game and generated research under ignored directories. Do not alter the supplied disc image. Verify the pinned target before analysis or building.

Report original-image baseline success, individual function matches, complete source-build results and runtime tests separately. Do not count original bytes as reconstructed source.

Preserve symbol evidence and unknowns. Do not invent types, original names or source boundaries to get a match. Keep experiments in scratch/.

Run checks appropriate to the change. The original-image baseline command is `python3 tools/baseline.py`; the source build is `python3 tools/reconstruct.py`. Validation tests are `python3 -m unittest discover -s tests -v`. Read `docs/initial-audit.md` before changing splitting, file-map or baseline tooling: dtk's own per-file ranges are wrong for this target, and `tools/file_map.py` must keep its strong rules exact against verified units. Read `docs/Dolphin.md` before adding SDK units. This target is compiled with Metrowerks CodeWarrior; unit manifests default to the `GC` compiler family. A working compiler profile is not proof of the original compiler release. Read `docs/Roadmap.md` for candidate tools and the staged plan, and `docs/ReuseMap.md` to choose the next target.

Progress counts verified matching source-built function bytes against all 1,414,572 original executable section bytes (`.init` and `.text`). Preserve the separate reconstructed-game and restored-library categories, upstream attribution and licenses. Never count original context, linker tables, data bytes, discarded functions or overlapping ranges toward code progress.

Check the repository-local commit identity before committing; never inherit a personal identity without authorization. Preserve contributors' pseudonymity and upstream attribution. Follow docs/Progress.md when changing source, headers, configuration or tools: regenerate the public snapshot only after complete local verification. CI validates a snapshot and must not be described as rebuilding the game.

Before choosing work, read docs/Remaining.md: it sets the priority order (collision, scripting, AI and animation first; multiplayer last) and lists which source forms are accepted or declined.
