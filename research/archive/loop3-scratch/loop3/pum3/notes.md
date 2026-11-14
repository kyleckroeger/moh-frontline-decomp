# pum3 notes (2026-11-12): PopUpMessageHandler ctor

Fix: in CreateUMStructFromFile's loop 'extra = m_messages[i].m_extra = entry[0];' (chained assignment)
-> ctor 0, out-of-line Load 0 (var.py). Unit source: scratch/loop3/pum3/pumhandler_ctor.cpp (landed
member names, extern umTable, no subtitle-queue globals). Probes: best0.cpp (pum2 names), va..vj.txt.
Inert: inline helpers for extra/advance (worse), register/volatile, Fill helper (58+), new-statement forms.
- LANDED 78daa000 (pumhandler_ctor, 988 B).
