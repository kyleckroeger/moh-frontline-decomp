# Contributing

AI-assisted work is welcome. Contributors are responsible for verifying proposed source against the original instructions, data and available symbols. Record AI assistance and unresolved assumptions in the change description.

- Keep original executables, disc images, assets, complete debug exports and tool binaries under ignored `orig/`, `build/` or `scratch/`. Commit scripts, source and focused evidence rather than bulk dumps.
- Distinguish recovered names from inferred descriptive names. A symbol's name or address does not establish a complete class layout or original source-file boundary.
- Choose compiler settings from evidence at source-unit or library scope. Do not patch instructions, insert assembly to substitute for compiled C/C++, or exclude differences to manufacture a match.
- Preserve the complete-image comparison when introducing source. Record what was compiled, which toolchain and settings were used, and which exact target bytes matched. Original-image relinking earns no decompilation credit.
- Keep source readable. Put compiler experiments and unresolved hypotheses in evidence notes, not misleading comments about original intent.
- Before accepting reconstructed units, review their behavior, types, boundaries, data ownership and comparison coverage. A byte match alone does not establish source fidelity.

Check [research/](research/README.md) before starting on a function: most have earlier attempts, and it has the logs, drafts and tools. Add your own findings there, including what didn't work, in the same pull request.

Open an issue to coordinate a unit or research topic, then submit a focused pull
request. Include the target revision, compiler/settings, commands run, match result,
progress change and remaining uncertainties. Documentation and tooling contributions
are useful even without a local game image; clearly label anything not verified.

For source, header, manifest or build-tool changes, run the complete source build
and tests, then refresh the public snapshot with
`python3 tools/progress_report.py --capture`. Then run `python3 tools/sync_docs.py`
and update any hand-written doc the change affects, in the same commit. See
[Progress.md](docs/Progress.md).
CI checks the snapshot and runs tests that do not require originals; local
original-dependent tests skip on public runners. Maintainers must review snapshot
updates and reproduce byte verification before accepting new source credit.

Pseudonymous contributions are welcome. Check your commit author/email before
committing and use your own GitHub noreply address if desired. Never remove upstream
author or license notices. AI assistance must be disclosed in the PR, along with
the checks you performed; generated output is a proposal until verified.

Original contributions are submitted under [CC0 1.0 Universal](LICENSE), only for
rights you are entitled to dedicate. Imported material and adaptations retain
their applicable upstream terms: preserve notices and record provenance, including
any licensing uncertainty. See [licensing scope and third-party notices](docs/Licensing.md).
