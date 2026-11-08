# frontline-decomp-setup

Agent working notes, kept as written; dated claims may be out of date.


The user contributes to lifewillbeokay/moh-rising-sun and wants this repo (MoH Frontline, GameCube GMFE69) to closely mirror its structure and CC0 tooling. Scaffolded 2026-10-04: baseline relink byte-identical, zero-unit reconstruct verified, 0% snapshot for decomp.dev.

**Why:** consistency with the sibling project the user already works in.
**How to apply:** prefer Rising Sun's conventions (tools/*.py pipeline, config/<ID>/ manifests, snapshot-based CI) over dtk-template. Target is Metrowerks CodeWarrior (not ProDG); dtk per-file splits are untrustworthy (see docs/initial-audit.md). Disc image: Windows Downloads, "Medal of Honor - Frontline (USA).iso" (converted with Dolphin).

GitHub repo: kyleckroeger/moh-frontline-decomp (renamed from moh-frontline on 2026-10-05; origin updated). CI runs "Verified progress snapshot" on push.
