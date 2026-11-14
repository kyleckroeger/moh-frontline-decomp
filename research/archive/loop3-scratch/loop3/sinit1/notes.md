# sinit1 notes (2026-11-12): __sinit_pumhandler_cpp

Result: 0 on the 5th probe; landed as the new file-final unit `pumhandler` (src/game/pumhandler.cpp);
the old `pumhandler` unit (PopUpMessageHandler dtor) renamed `pumhandler_dtor` (an __sinit must be
compiled from a source named <file>.cpp, port_unit cannot match __sinit_<other>_cpp; dmmorph precedent).
Tools: sc.py [-v] src... (scores the compiled __sinit_probe_cpp vs the original, -v prints side by side + symbols).
Findings:
- extab record of the original sinit has no actions (gprs 9, fprs 1); the objects come from the two
  __register_global_object calls: umTable (+12, record +0, __dt__17CUserMessageTable) and
  g_subtitle_queue (+92, record +80, weak list dtor). lastString (50 B, pumhandler_add) sits at +28.
- "0.0f in f31" was wrong: f31 is saved and stored but never loaded = uninitialised sentinel times,
  scalar-promoted only when QueuedSubtitle's colour is four plain bytes (s5). Any CColor member: 38 lines.
- CUserMessageTable ctor: store order +0,+8,+4,+12 = body assignments m_messages, m_index, m_text, m_count
  (the landed out-of-line ctor), not an init list.
