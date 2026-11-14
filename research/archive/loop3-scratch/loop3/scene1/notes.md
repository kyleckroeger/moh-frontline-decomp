# scene1 notes (2026-11-12): CScene::Draw and the CScene constructor

Tools: s.py (real score: `python3 s.py d|c files...`), rep.py (string-replace variant maker), toy.py (compile a toy file with
scene.json flags and print a function), ext.py (extab/extabindex of a compiled draft).

## CScene constructor: LANDED ea52bfd7 (scene_dtor += ctor, 900; one_unit VERIFIED, pushed)
- KEY: the dwi containers need their (inline) DESTRUCTORS. With -Cpp_exceptions on, each constructed member gets a cleanup
  record in the ctor's extab, and that alone moves the constants 0/1/2 into r27/r28/r29 and the list sentinels into
  volatile registers (92 -> 0). Every init-list/body/list_node form was inert without them (kgrid.py, l1/l2).
- Tip: compare the original's extab record (objdump -s -j extab at the extabindex's third word) with ours: action
  records name the destructors the original source had.

## CScene::Draw: 48 -> 39 (draft updated)
- LOGIC FIX: the command loop passes the node: SModCommand is {void (CScene::*)(ISceneNode*); ISceneNode* m_node} and
  the call is (this->*c.m_fn)(c.m_node) (scene_update.cpp's form). Using scene_update.cpp's inline ProcessCommands()
  (m_drawing = false + the loop + clear) gives 39.
- Left: the camera+192 hoist (orig r29 = camera+192 computed before the screen call, one more callee reg r25).
  Toy findings (toy*.cpp): MWCC forwards a single-use address local (ref/pointer/const pointer/register/inline param/
  getter, any position) to its use; it keeps it computed early only when (a) the variable is used TWICE after the
  call (t4/z5: then the addi is scheduled before the mtctr exactly like the orig), or (b) the use is in another basic
  block (z4/q3), or (c) the argument contains a branch (w3/x3, evaluated before the calls, left to right). No natural
  Draw form found with a second use or a branch that leaves no trace. Screen call inside the ctor args
  (natural, as in Moh2.cpp) = 150 (length -1); the screenTM local keeps 39.
