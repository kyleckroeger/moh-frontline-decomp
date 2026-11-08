# port-unit-category

Agent working notes, kept as written; dated claims may be out of date.


When drafting a unit with `tools/port_unit.py`, pass `--category reconstructed_game` for any source under src/game or src/ea that has no upstream. The tool defaults to `restored_library`, and `--like` copies only the compiler settings and provenance, not the category. Also delete the false `adapted_from` it adds.

**Why:** On 2026-10-07, 142 game/EA fragments turned out to be recorded as restored library code (fixed in commit e599a11). That skews the separate game and library categories that AGENTS.md says to preserve.

**How to apply:** Check `category` in every new manifest before `scratch/land.sh`. Related: [[game-code]], [[autopilot-loop]]
