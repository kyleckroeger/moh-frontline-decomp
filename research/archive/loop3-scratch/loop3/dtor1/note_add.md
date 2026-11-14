- pumhandler: the clear() form makes the list fragments' pair exact in one compile (pumhandler_list's
  ~list<QueuedSubtitle> and pumhandler_list_reserve, pw/pumhandler_lr.cpp); the whole-file draft is
  not written yet. world_object (callee registers: the loop calls ~CCompartment): with clear() the
  out-of-line ~list<CCompartment> is exact and reserve's node/next keep the right relative order, but
  both sit one register lower (r26/r25 for r27/r26) whatever the clear() spelling (pw/wov.py, 7
  forms): the second inline level seems to add one ranked value. Open.
