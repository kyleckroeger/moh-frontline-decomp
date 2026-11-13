# Cross-lane tricks found by the parallel agents (append; newest last). Try these on FP/colouring near-misses.
- (lines) inline fabs: '#undef fabs' + 'inline double fabs(double x){ return __fabs(x); }' fixed the line-test colouring (FindIntersection excepted: keep the macro there).
- (lines) Named 'struct { float x, y, z, w; } c;' inside the CVector3 union; access components through it, not v[].
- (lines) Per-axis products in REVERSE order = a 3-float ctor 'CVector3 l(a.Dot(v), b.Dot(v), c.Dot(v))' (args evaluate right to left).
- (lines) A small value-returning inline (Compare(sep), Radius(...)) fixes FPR colouring / out-of-order param regs that declaration order can't.
- (lines) Hoisted named float locals: declare them in order of the orig's register number, low to high.
- (lines) Mirrored temp stack slots = the body sits in an inline helper. 'ble L; b L2; L: fmr' min = 'a > b ? b : a'.
- (lines) extab cleanup -> weak __dt__6CLine3Fv = CLine3 has an inline empty dtor; port with --weak-duplicate __dt__6CLine3Fv.
- (lines) When declaration order doesn't move FPR colouring, the order of the COMPUTATIONS often does.
- (soldier) Give the class view of a called-through global a realistic size (an empty view makes it small-data and breaks the prologue).
- (soldier) 'extern const' arrays drop the load-after-store ordering. Inline accessors (GetPlayer(), static inline GetListStart()) fix GPR colouring.
- (soldier) Code duplicated across switch cases = one inline helper with reference outputs (int& k, int& i). 'flag = offset = 0' (flag unsigned char) = one shared li.
- (soldier) lfsx with a full offset = float indexing '((float*)this)[i*3+6]'; struct form gives add + lfs.
- (parks) 'bcc L; b M; L:' = conditional expression whose first operand is a copy CW coalesces away.
- (parks) CW folding '+31'/'+12' too early: split the add into its own statement or pass the value through an inline param.
- (parks) 'li r3,2; cmpw r3,r0' = a long local compared with an int member.
- (parks) 'li a,0; mr b,a' at a loop head = two strength-reduced IVs (write i*k) or a reused loop var; 'x = y = 0' = shared zero.
- (parks) Reusing parameters as working variables (cursor, remaining size) fixes param/local register swaps.
- (parks) Copy groups of 4 want int temps; length loops 'while (n >= k) { ...; n -= k; }'. Masks as '~0x20000000' (not unsigned hex).
- (parks) A dead 'v = 0.0f;' before a loop fixed constant load order.
- (parks) scratch/par/parks/hill2.py: declaration-order hill climb (single moves + pair swaps); repeatedly took 100+ line reg diffs to 0.
- (parks) ALWAYS rawcmp before landing: fast.py/ss.py hid a wrong branch target and string-address scheduling.
- (bigfiles) Moving a plain 'float x;' declaration to the top of the function changes FPR use (fixed CPropertyAnimLight::BeginUpdate).
- (bigfiles) Named locals declared at the top ('CVector3 dir; CVector3 local;') fixed every stack slot in PeekLeftRight.
- (bigfiles) One CVector3 local reused via an inline Set() = all three vectors share one stack slot.
- (bigfiles) Real members instead of pointer-offset casts stop CW caching 'object+48' in a register. Small inline helpers (Mul(a,v,i)) change surrounding codegen.
- (bigfiles) Gotcha with bigfiles' s.sh: export TM/FN/A/B in the SAME shell call, or it silently scores against player_tm.json ("len N 0").
- (lines) A value kept in f1 (or r3) to the very end of a 'void' function = the function RETURNS it (mangled names hide return types). CVector3::Normalize returns the length: 25 -> 0.
- (soldier) Script-arg 'shared knot' (count/top register swap in BIFunc_*): read args explicitly with 'int count = g_pBuiltInFunctions[i].parameterCount; int a = *(*stack - (count - 1));' and test/switch through a plain copy 'int selector; selector = a; switch (selector)'. Landed MechSetPathFlag 3730b0a1, GetWeaponType cffa9212.
- (soldier) Old permuter dirs can hold unnoticed score-0 outputs: scan scratch/perm/*/output-0-*/score.txt for 0 on uncovered functions.
- (bigfiles) Placement-new temp in the wrong reg ('mr r3,rM; beq; bl ct; mr rO,r3' vs orig 'beq; mr r3,rM; bl; mr rM,r3; mr rO,rM'): declare the memory pointer INSIDE the if that tests the source member ('if (m_free) { T* memory = m_free; ...') instead of testing the local (AllocateObject 18 -> 0).
- (bigfiles) Stack slot order of temps/locals: moving a block into ONE static inline helper reorders them (inline locals/temps are allocated after the caller's). Create(void*) slots fixed this way.
- (bigfiles) Several returns of a pointer that the original funnels through one callee-saved reg + a shared 'mr r3,rX' at the exit: put the loop with its returns in a static inline helper and 'return Helper(...)'.
- (bigfiles) Declaring a pointer local with a BASE-class type ('CParticleSystem* ps = pool.Alloc()') swapped two callee regs into place where decl order did nothing.
- (soldier) 'x - (int)x' passed as an argument vs 't -= segment; f(t)': the compound form changes int->float constant FPRs/scheduling (FireProjectileAtPath 51de5eaa).
- (bigfiles) A 1-bit bitfield written as a halfword (lha; rlwimi; sth) while assignments give lbz/stb: initialise it in the constructor's mem-initialiser list ('m_reverse(0)').
- (lines) An x component re-loaded after a Dot (forward.x / up.x read twice) = the vector is read through a reference/pointer in the original ('const CVector3& fw = forward;'): the offset-0 member path then CSEs.
- (lines) 'if (p) *p = v;' checks on &local survive only when the inline has a 'return' inside that branch (Eberly-style early returns); otherwise CW folds them.
- (lines) Inline-local temp slots come out in the wrong order? Wrap the inline call in another tiny inline (sqrtf(SegmentSqrDist(...)) inside 'Distance(...)'): nesting changes the temp allocation order.
- (lines) Operand order of a fused dot inside a helper: pass the dots as float ARGUMENTS (helper(dir.Dot(axis), diff.Dot(axis), ...)) instead of computing them inside.
- (lines) dmh.py (scratch/par/lines): dependency-safe statement-move hill climber; big wins on long FP blocks (box-capsule 205 -> 186 -> 138).
- (parks) Ctor args read BEFORE operator new = a static inline factory 'static T* Create(a, b) { return new T(a, b); }' (args become inline params; also fixes their colouring). extab of 'new T' with an inline ctor: declare the bases' dtors non-inline (DESTROYBASE actions) and the class operator delete (DELETEPOINTER) exactly as the original's weak copies exist.
- (parks) A loop whose body sits out of line AFTER the following if/else = the found-handling is INSIDE the loop: 'for (n = head;; n = n->next) { if (n->size >= size) { ...; break; } }'.
- (parks) A reassigned parameter colouring wrong: route the new value through a tiny inline ('size = Align(manager, size);'); a dead 'value = 0.0f;' before float poll loops fixes the 0.0f/int-to-float constant load order (HandleButtonPress/HandleAnalogButtonPress).
- (bigfiles) A this/local callee-reg swap that decl order can't fix: pass 'this' to the inline helper as void* and cast inside ('CParticleSystem* ps = (CParticleSystem*)p;') - the conversion gives a fresh vreg (UpdateSystem 38 -> 1 swap).
- (soldier) Decl-vs-assignment order brute force (declare in one order, assign in another) found 0 for a 3-float inline distance where all 36 'float d = ...' orders failed (GreedySearchAwayFromPoint ae587532).
- (parks) A list/sentinel address that orig keeps in a real volatile reg (r3/r5) where ours uses r0: copy it into a NAMED pointer local inside the inline ('ARAM_NODE* pos = (ARAM_NODE*)list;') - ARAM_NEW_pool 0ebea648. Also: putting the shared-zero/IV logic in an inline with its own locals turned a constant-propagated 'li' back into the orig's register copy (asyncserver resolved store).
- (soldier) A reference param of an inline (T& pos) bound to obj->m_pos keeps &pos in a callee-saved reg; when the orig re-forms 'addi rX,obj,off' per use and loads off(obj), pass the object pointer instead (IsTargetInLineOfSight 8551aa92). A function weak in the original = mark its definition inline.
- (lines) A product 'r * r' wrapped in an inline 'Sq(r)' fixed a 2-register swap (GetDistanceFromPlane), like the value-returning inline trick.
- (lines) Two '&m_head'-style address expressions are CSE'd into one register; to get a FRESH addi (orig recomputes), write the second as '(T*)((char*)this + off)' (list reserve). Conversely one shared local passed to two inlines forces sharing (AddSubVolume).
- (lines) A struct built from three computed components: try the 3-float ctor / 'v.Set(x, y, z)' (args evaluate right to left) instead of member-wise stores (Orthonormalize 68 -> 21).
- (lines) Re-associated products 't * x * y' (CSE with tx = t * x) instead of 'tx * y' changed FPR colouring (BuildRot 19 -> 2).
- (bigfiles) Shared-zero 'li rA,0; mr rB,rA' that every function-level spelling const-props to two li's: move the variables into a static inline with its OWN locals ('unsigned int hits = 0; int location = 0;' even plain = 0 works there); then volatile reg order via a read temp 'unsigned int v = values[j]; if (v > hits)' (GetCumulativeStats f1f3e506; CPlayerObject::OnCollision got the copy too via an inline PickUp helper).
- (soldier) FPR numbering wrong only in a second, later copy of a vector computation: REUSE the first local ('direction = CAIFilterRealVector3(a, b); direction.Normalize();') instead of declaring a new vector (ProvidesCover 43 -> 0, 12393d27).
- (soldier) An aggregate-initialised table that sits ABOVE nested-block locals (matrices) in the frame is a caller-level local, not an inline's: inline locals are placed below the caller's (CheckLineCollision).
- (parks) Global-record setup code: compute derived fields FROM the stored fields ('g.end = g.dest + g.size; g.endAligned = g.end & ~31;') instead of from params/locals - CW keeps the source value numbering and the callee-reg colouring follows (StartNonAlignedAyncRead 23 -> 1). Signed cmpw on pointer-like fields = the fields are ints.
- (parks) A field load the orig RELOADS where ours CSEs: read it in the earlier test through a different access path (the buffer param that was stored there, or a local 'T* p = &global' for the first test only); the later test via the global then reloads. Operand-register swaps in 'a - b + 31': try '-b + a + 31'.
- (parks) A load scheduled one slot too EARLY that no statement/declaration order moves: make the thread-shared fields VOLATILE (per field): volatile loads keep source order relative to each other (ReadFileThreadFunc ec2292b1: request words + handle type). Also: a function that is local in the original but called from another unit must be DEFINED static (port_unit discards a global definition with "Cannot discard a symbol present in the target").
- (bigfiles) Whole-function callee-reg rotation of PARAMETERS (orig puts some params high, others low; decl/copy tweaks do nothing): move a self-contained early block (a slot search) into a static inline helper taking (section, placement) - the param priorities then match (AddPopUpMessage 138 -> 93). Then: pass the helper the member expression again ('FindSlot(m_messages[placement], ...)' rather than the 'section' local, permuter hint), inline local decl order, and the for-increment ORDER ('other++, k++' vs 'k++, other++') sets IV increment order + regs.
- (bigfiles) Function-static buffer in .data (not .bss) in the original: give it an explicit initialiser ('static char buffer[80] = "";').
- (bigfiles) 'bool f = n == 2 ? a ? 1 : 0 : (bool)(b1 || b2) != 0;' reproduces a double neg/or/srwi (!=0) only on the else path (DrawViewportEdge).
- (parks) Globals named vg*/written by callbacks or other threads: try VOLATILE first when store/load ORDER or address-materialization order is off (iPAD_init 4e8fc2ea).
- (parks) Shared-zero register (one 'li rX,0' feeding stores AND a call argument): put the tail in an inline with its own locals and give the argument variable the callee's parameter type (char) - PATHI_crossfadetracks 608c88ed.
- (soldier) Orig compares a register that holds a known constant ('li r31,0; cmpw r31,rX' / 'li r5,12; cmplw r5,r4') where ours folds to cmpwi/cmplwi: make the value reach the compare through an increment inside an inline ('index = -1; ... Next() { index++; ... }', or a search loop 'for (p++; p < end; p++)' instead of passing 'p + 1') - CW folds such values only late (GetClosestUsableCoverPoint 039530a3).
- (bigfiles) A CVector3 with a USER copy ctor (pair cast) makes an extra 16-byte stack temp when a named vector is passed by value through
  two by-value levels (CLine3(CVector3 s, CVector3 e) { SetSE(s, e); }); the implicit union copy (weapon_bullet.cpp view) does not. If a
  merged unit's view is fixed, land the function as its own fragment with the other view (DetectGround).
- (parks) A callee-reg swap on a 'load = g.load; ... load->reads[0].x' local: point the local at the SUB-RECORD actually used ('read = &g.load->reads[0]; read->x') - CW folds the offset and the colouring changes (SNDBANKI_asyncservice e8b97d00).
- (lines) Temporaries' stack slots vs copy schedule: pass by-value temporaries (m.GetRow(i)) as const-ref ARGUMENTS of an inline (Dots(r0, r1, r2, ...)) - all copies happen before the uses and slots come out in argument order; binding them to 'const T&' locals instead allocates bottom-up in binding order. Results through 'Set3(a, b, c, x, y, z)' (args right to left) fixed several FPR colourings (IsPointInTrigger a4233e3e).
- (lines) Size '/ 16.0f' (becomes * 0.0625) keeps the converted value as the FIRST fmuls operand; '* 0.0625f' puts the literal first.
- (bigfiles) Two callee regs holding the same global address ('addi r30,&g_scene; ... mr r31,r30'): call through the global directly at every use (no 'CScene* scene' local) and hoist the loop decls (camera, matrix ptr, i, player) - Gameloop_Loop. Check the global's view has its real size first (a too-small class is SDA-addressed).
- (bigfiles) Hoisting plain declarations ('int pos; unsigned char* index; char* text;') to the function top in a brute-forced order fixed a callee-saved rotation where declare-at-use could not (CUserMessageTable::Load 22 -> 13).
- (soldier) CW inlines at most 3 levels deep. A callee inlined in one function but CALLED in another that reaches it one level deeper = the depth limit; if the orig stops one level earlier than yours, add an inline wrapper level (GetAIStatusVar/GetVarValue 3bfb724a). Also check 'w' binding in the ELF symbol table: weak = an inline function.
- (soldier) Shared constant 'li rA,-1; mr rB,rA' for two variables: same as the shared-zero trick - a static inline with its own '= -1' locals copied to reference outputs (OnCollision dd274d26).
- (parks) An inline param copied into the wrong volatile reg ('mr r5,rX' before the inline body): let the inline work on its OWN local ('int result = node;' ... 'return result;') and leave the parameter untouched (PATHI_enternode remapnode). A value computed into r0 then 'mr rN,r0': make the inline return a narrower type (unsigned char) (enternode pathrandom). 'x = a[(p = f())->k]' assignment-in-expression put p straight into its final reg.
- (parks) Shared-constant reuse (one 'li r0,1' feeding both '1 << j' and a flag; the flag's zero reused for a store): move the per-item loop body into a static inline with its own locals (PATH_fade 2931a119).
- (bigfiles) Compare of two loaded ints whose load ORDER/regs is swapped (canonicalised: operand order has no effect): read both through tiny inline accessors 'int GetViewport() const { return m_viewport; }' (CProjectileBullet::Draw a2ebe550).
- DECLINED by user 2026-10-25: padding arrays with unused elements to move a stack slot (XAFSTATE saved[2]). Don't ask again.
- (parks) abs() temp colouring off: '__abs(x)' vs 'd = x; if (d < 0) d = -d;' give the same srawi/xor/subf idiom with different register order - try both. A signed bitfield used twice (test + arithmetic): read it into a 'short' local (keeps the orig's extsh, and changes the colouring of neighbours) (PATHI_serviceproject 084a00dc).
- (soldier) BIFunc param 'void* object' in the wrong callee reg: read it through a typed local 'ISceneNode* node = (ISceneNode*)object;' used at EVERY use (AEMSEvent 31 -> 17). A stack-held result's '-1' temp register: move 'int result = -1;' between the argument reads (17 -> 0, 2616e75b).
- (bigfiles) A struct copied as WORDS from its stack home (incl. uninitialised words, 'stb' then 'lwz' of the same word) plus a null check around the init = placement new of the container node with a ctor 'list_node(const T& v) : m_prev(0), m_next(0), m_data(v) {}' ('new (node) list_node<T>(value)'); member-wise assignment gets scalar-replaced (AddNodeExtents 69e74f86).
- (bigfiles) Lerp colour channels without fmadds and with (d * t) operand order: 'r = to - from; r *= t; r += from;' on hoisted float locals (CFaderControl::Update b9a10f83).
- (lines) 'mulli rX,6; slwi rX,3' for a 48-byte record = an array of 8-byte pairs indexed 'base[i * 6 + k]' (int local for i*6), not a 48-byte struct (that gives mulli 48) - InitBulletDecalFromCollision 4aa6607d.
- (lines) A local matrix filled through an operator= CALL with no constructor/InitClass check = inline copy ctor 'CMatrix(const CMatrix& o) { *this = o; }' used as 'CMatrix m(src)'. 'slwi r,16; srawi r,16' = '(d << 16) >> 16' angle wrap (a (short) cast gives extsh) - DMClusterSynthesizeMatrices 42a2b998.
- (soldier) Callee-saved colouring of locals: split 'T x = e;' into declarations at the top + assignments in the orig's load order, then hill-climb ONLY the declaration order (scratch/par/soldier/hill.py on the decl block): earlier-declared locals get higher regs (r31 first). Solved RegisterTargetSearch (10 -> 0, ed51ee29) and RegisterTargetCheck 87 -> 45. Parameters always get the LOWEST callee regs in our builds (probe soldier/probe2) - an orig with a param in r31 has something else going on.
- (lines) Volatile-reg swap between a walking pointer and a compiler IV (i*20 offset): write the pointer as indexed 'part[n]' / 'index[n]' (strength-reduced IVs are numbered after the outer IV); unconditional increments that the orig schedules after the IV updates go in the for-increment clause 'n++, parts++, count++' (DMGeomBuildPartList b9f4538e).
- (parks) Two volatile regs swapped around a product ('neg rA; mullw rB,rA,s' colour): 'int offset = d; offset *= stride;' (permuter hint: copy then compound multiply) fixed both colour and mullw operand order (YUV_convert 897e0ca2). A Y-row offset kept as an explicit IV took a high callee reg; writing 'base + i * width' and letting CW strength-reduce gave the orig's low reg.
- (parks) port_unit --like <manifest copy outside config/> writes an "adapted_from" with "manifest" that fails test_provenance for reconstructed_game units: delete the key.
- (parks) Bit-packing 'a<<24 | b<<16 | c<<8 | d' where the orig has only ONE rlwimi then slwi/or: write it as accumulation 'p = a << 24; p |= b << 16; p |= c << 8; p |= d;' (the single expression inserts every byte with rlwimi).
- (lines) Orig keeps a register copy of a placement-new result ('mr r7,r10' and the rest of the loop uses the copy): 'CPartBin* bin = new (&m_parts[i]) CPartBin; CPartBin* part = (CPartBin*)(void*)bin;' then the first fix-up through bin, the rest through part (CDMesh::Init 83185a4f, parked since 10-09).
- (bigfiles) Two strength-reduced loop pointers whose INIT order is wrong (regs right): route the array address through a second pointer declared at function top ('int* row; ... row = arr; fill via row; int* p = row;') - permuter find, CheckIfMovieAndMedal de8477f7. Also: run autoperm on C89-style drafts ('int k; for (k = ...') - 'for (int k' makes most permuter iterations fail (DeclList).
- (bigfiles) A float member scaled conditionally whose result is stored again after the merge and used via frsp: write it as a ternary 'v = cond ? 0.5f * v : v;' (CPlayerWeaponObject::Draw).
- (soldier) PARAMETER stuck in the LOWEST callee reg while the orig gives it r31: look at bool conversions of call-crossing locals - '(bool)flag' instead of 'flag != 0' as a call argument flipped the int** stack param from r25 to r31 (RegisterTargetCheck 1ce1c7bb). Probes: params normally rank below all locals; one extra call-crossing node also flips it (pr7/p16.cpp), so try neutral rewrites that change the graph (casts, inline wrappers around the final call).
- (parks) 'bne then; b join' (empty else block kept) = an if/else whose ELSE re-assigns the value the variable already holds ('if (!(w & m)) valid = mode & m; else {...}' after 'valid = mode & m;', or '{ T n = name; if (!p) n = name; else {...} name = n; }'). CW coalesces the copy but keeps the block. Plus: 'n = p; n++;' (not 'n = p + 1') kept p in a volatile reg and computed the result before a following call (SNDI_validrendermode ada3e6e9, TLT loaders d23c99b4).
- (parks) Return value computed separately from a stored copy ('addi r0,r3,1; addi r3,r3,1; stw r0'): keep an 'int result' computed from the expression, store it, and increment both ('++s[0]; ++result;'); reading the result back from the global CSEs them (iSNDrandom tail).
- (bigfiles) A huge function whose body repeats itself 2-3 times with calls only at the deepest level = plain SELF-RECURSION that CW inlines two levels deep (DrawNodeShadows 1600 B from a 4-line source, 85f2e2e9). At the deeper levels inlines hit the depth limit: helpers called there must be real (user-written) inline functions (e.g. a user CVector3 copy ctor), not implicit copies or direct member reads.
- (parks) DECLARE in one order, ASSIGN in another: 'int channels; int rate; int bits; rate = ...; bits = 0; channels = ...;' - declaration order sets the register numbers, assignment order sets the load/copy order. Fixed SNDSTRMI_calcdatarate (9497c031) and SFILTER_echo's inline param copies (30f556d4: inline params renamed, locals 'src, dst' declared, assigned 'dst = out; src = in;').
- (lines) A DEAD 'b end' right after a switch dispatch (b X; b X) = a case block that empties only LATE: e.g. 'case 0: if (cond) break; break;' or a coalesced copy 'id = none' (SetPlayerCollision). Plain 'case 0: break;', empty statements, dead stores and default placement are merged early (RunEnvironmentCollisionChecks 2e6ee18f).
- (soldier) sqrtf result in a NEW FPR with an 'fmr fN,f31' on the x<=0 path where the orig reloads the volatile y straight into x's register ('lfs f31,8(r1)'): take the root IN PLACE through an inline 'void SqrtInPlace(float& x) { ...same body...; y = (float)(x*guess); x = y; }' (no return) - PropagateSoundToAllObjects 14 -> 0.
- (bigfiles) Callee-reg rotation between a walking pointer and hoisted loop invariants: index the array inside the loop ('T* c = &arr[i];') instead of 'c++' - the strength-reduced IV is created after the invariants and takes the higher reg (CFont::Link 62 -> 0, 282f3a06). The reverse (c++ pointer) when the orig pointer is the lower reg.
- (lines) list reserve fresh sentinel: 'head = end().m_node;' with end() returning list_iter((list_node<T>*)((char*)this + 16)) - landed SExtent/SRequest reserves (2955b3cc, 4024703b).
- (lines) Shared zero 'li r5,0' for member clears AND a loop's initial 'next' (ours: two li): move the whole set-up block (clears + free-list chain loop) into a member inline 'init_free()' - function-level spellings (m_free = next = 0, casts) fixed the zero but rotated the parameter callee regs (alarm_list_ctor ff74409b). Element-wise copy ctor = strictly sequential lwz/stw copy.
- (bigfiles) FPR colouring wrong in an EARLY block that no local change fixes: the cause can be LATER locals - hoist the declarations of a later branch's float locals (far/near/aspect of the perspective if/else) to the FUNCTION TOP and brute-force position x which vars x order (CPlayerWeaponObject::Draw 4 -> 0, c382731c). Use bigfiles/t6/rd.py for register-exact diffs.
- (lines) Float call args computed in the wrong order (CW evaluates args right to left): take them into locals in the original's order first ('float q0 = c.Dot(diff); float q1 = c.Dot(e1); separate(r, q0, q1, ...)'). Copying a by-ref result's fields into locals before the call fixed a one-load schedule too (CCDBTriangle box test 029e8fe5).
- (parks) Inline allocator whose 'aligned' copy of the size param must NOT coalesce with it ('mr r31,r5' kept, size in a volatile reg): hold the size in a LOCAL of the CALLER and pass the local ('unsigned int size = ...; p = Alloc(size);'). Passing the expression directly gives the coalesced form (both occur in animemot_copy c574d096). Statement order inside the inline ('amount += size' before 'count++') fixed the remaining load order.
- (parks) Two pointer PARAMS swapped (orig src r31/dst r30): change the pointer TYPE fed to an inline helper (void* -> unsigned char* in AnimCopyData) - CopyStateDef 160 -> 146. Also: a draft's permuter run found 'unsigned int aram' (vs unsigned long) fixing one callee reg in UploadFramesToARAM.
- (soldier) Param-rank flip without bool: a comma-initialised for header 'for (i = 0, kept = 0, total = 0; i < n; i++)' (instead of 'kept = 0; total = 0; for (i = 0; ...)') moved all six params of death_cb from r23.. to r26..r31 as in the orig. Also: a loop's output counter in its OWN hoisted variable (not reusing an earlier counter) fixed a whole loop's volatile colouring (death_cb 97 -> 12).
- (bigfiles) Register COPY the orig keeps ('mr r5,r4' hole = it.m_p, later reads through the copy) where CW coalesces ours: initialise through a void* round trip 'T* hole = (T*)(void*)it.m_p;' and compute the other derived value from the source ('p = Next(it.m_p)'). Struct local copied MEMBER-WISE (lbz/stb of a byte field) where ours block-copies words: declare then assign ('V v; v = src;'), implicit operator= is member-wise, implicit copy ctor is a block copy (both hash_map erases, df9bcec7/cee2ccff).
- (lines) Operand order of a CSE'd re-associated product (fmuls f, B, A instead of A, B): split it 'float p = t * x; p *= y;' - the compound statement keeps the CSE value first while 't * x * y' and 'y * (t * x)' both swap it (BuildRot 528601ae).
- (lines) Shared zero 'li rA,0; mr rB,rA' for two variables that must sit in the HIGHEST callee regs: make them params of the static inline with default arguments ('T* messages = 0, unsigned int count = 0') and call without them - inline locals initialised to 0 also share the li but take the lowest regs; param order sets which gets rA (TriggerActivateMessages aed4ce61).
- (bigfiles) Two __fabs operands loaded in the wrong order ('fcmpo z,x' but orig loads/fabs x first): use an inline 'double fabs(double x) { return __fabs(x); }' - inline-call args evaluate right to left (CProjectileBullet::Init b83565f0).
- (parks) Loop UNROLL FACTOR differs (orig x2 with 'srwi./andi.' remainder, ours x8 'h-8' form or none): CW picks the factor from the body's IR size, so a no-op cast in the body ('d[k] = (unsigned int)s[k];') can switch it (form_component_prediction 499b41ef). Manually written-out rows (16 statements) vs an inner constant loop also differ: the inner loop is fully unrolled but then blocks the outer unroll.
- (parks) A unit whose static helper must stay a CALL while auto-inline inlines it: compile the unit with '-inline on' (explicit 'inline' still honoured) - sdecmut_reftolpc 6538aaf3 (filter + initmut + reftolpc).
- (parks) Loop offset IV in the wrong callee reg vs a hoisted invariant: assign it from the counter at the top of the body ('o = i * 108;') instead of 'o = 0' in the for header + 'o += 108' - CW strength-reduces it to the same li/addi but colours it later (decodemut 4b8511a6). Try the reverse too.
- (lines) Two values of the same variable living in DIFFERENT regs where ours updates in place ('fsubs f7,f6,f10' orig vs 'fsubs f30,f30,f10'): the source likely never re-assigned it - inline the update as a repeated subexpression ('1.402 * (cr - 128.0f)' at each use, CW CSEs it into a new vreg) instead of 'cr -= 128.0f' (CLight::SetDefaultLight e7064024).
- (soldier) Callee-reg swap between a param-like local and a compiler CSE temp (its '*4' index): copy the local into ANOTHER existing local right before a later loop and pass the copy to the call there ('state = nextState; ... Register(.., state, ..)') - the extra live range flips the pair (NEXTSTATE cfcf75c6, permuter find after ~200 manual forms). Run the permuter early on 1-2 reg swaps.
- [bigfiles 10-27] FPR colouring left after decl-order sweeps: swap the STATEMENT order of two independent computations (UpdatePathMotionDestAndSpeed: 'along = dot(d,t)' before 'length = Dot3(d,d)' -> 18 to 0). Brute-force adjacent independent statement swaps.
- [bigfiles 10-27] Self-copy of a struct through a local ('CQuaternion q(f.rot); f.rot = q;' with user float copy ctor/operator=) reproduces "reload all words then store all" sequences (PatchUpMVDFile).
- [bigfiles 10-27] fast.py/rd.py hide float literal values: 0.01001001f vs orig 0.0100100115f (1 ulp) - run one_unit before landing; search the shortest decimal for the orig bits.
- (lines) FPR colouring of values produced by a value-returning inline with a local (switch 'float extent; ... return extent;'): use an output-reference form 'void Extent(int i, float& extent) const' with the locals declared first - fixed Case0's whole colouring (28 -> 0, eea430f1). Reverse of the 'value-returning inline' trick: try both.
- [bigfiles 10-27] By-value CVector3 param copied from a MEMBER as lfd a->f1, lfd b->f0, stfd, stfd: the one-by-one copy ctor can't produce it (alias -> interleaved or swapped regs); pass 'm.d' through an inferred init-list ctor 'CVector3(const CVector3Data& o) : d(o) {}' (CommitUpdate SetPos). Also: a block whose stack slots come out in inline-local order (bottom-up) = an inline helper (EjectCasing).
- [bigfiles 10-27] extab action with an out-of-line weak dtor (e.g. __dt__6CLine3Fv) for a named local = give the class an inline '~T() {}' and port with --weak-duplicate.
- (parks) Loop not unrolled where the orig unrolls x2 ('srwi. rX,n,1; mtctr' + 'andi.' remainder): the trip count was reloaded each iteration (short stores may alias the int count member) - load it once ('for (int j = out->m_count; j != 0; j--)' gives the orig's cmpwi+beq). Volatile colouring of a loop invariant ('fixed' in r0 instead of r6, forcing a count copy): move the invariant's computation INTO a static inline that holds the loop (BlendValues(out, &frame, weight) computes 'fixed' itself), then brute-force its decl x assign orders (BlendSkelFrames 89045502).
- (parks) Fragment calling a LOCAL function of the same file that is not yet in a unit: declare it extern (not static) and list it in the manifest's externals + local_externals; an undefined static declaration becomes a local undefined symbol and fails validate_object ('Unexpected compiler dependencies').
- (soldier) 'cmp; bgt L; b end; L: ...' (branch over an unconditional branch to the epilogue) = an EARLY RETURN whose value copy CW coalesces late: 'if (avail <= 1) return (T)avail; avail &= ...; return (T)avail;' (GetAvailablePeekPosition 36940b62). if/else, ternaries, switch, goto all give 'ble end'.
- (soldier) A value that sits in an argument register at a call with NO set-up instruction ('uninitialised' r5) = the argument is a value computed just before that happens to land there - check what last wrote the register (GetPeekLocation: types.m_bits passed as the COVERPOINT_TYPE).
- (soldier) Stack slot order: named locals (incl. block-scope ones in switch cases) are allocated first in declaration order, then ctor/return temporaries and argument copies in order of appearance, inline locals last. To move a 'CVector3(a, b, c)' temp up among the locals, make it a NAMED local built with the 3-float ctor (not '= f()' which adds a copy) (CThrownBullet::Init 81 -> 25).
- [bigfiles 10-27] A bitfield byte the orig RELOADS (lbz r0 each time) where ours CSEs it into a register across blocks: read one of the tests through an inline bool accessor ('bool IsDisabled() const { return m_397_40; }') - CW does not CSE a load inside an inlined accessor with a direct member load (BeginUpdate).
- (soldier) Callee FPRs of call-crossing temporaries ALTERNATE between similar blocks (first block f31/f30, later ones f30/f31, shifted by earlier webs such as float params). When an inline helper's return values get the wrong pair, use a MACRO for the helper and assign hoisted locals declared 'float x, y, z;' (not initialised at declaration): 'z = RANDF; y = RANDF; x = RANDF; v.Set(K * x, K * y, K * z);' (CThrownBullet::Init ae32d318).
- (lines) A CVector3 value that is (a) read via floats after a compound op: any INLINE-RETURNED temp (operator*, friend, Mul()) passed to +=/-= makes CW reload the x components of other vectors afterwards; an explicit local/ctor temp 'a -= CVector3(s*v.x, ...)' does not. 'T a = v * s;' is copy-elided only if a is never modified. A 'v; v = m.GetRow();' with a BY-VALUE getter and op= returning by value = getter temp read + dead copy of v (orig pattern in _matrix_rot_yzx and CPlane::GetIntersection).
- (soldier) autoperm fixup bug: a reference param used as '*ref.member' (e.g. '*collision.m_node0') is printed '*(*collision).m_node0' and fixup.py strips the '*' -> "Unable to compile base.c". Patch the generated <dir>/fixup.py second re.sub with a lookahead '\b(?!\s*[.\[])' (done in soldier/perm_hv).
- (soldier) FPR colouring inside an inline: hoist ALL its float locals into one declaration and brute-force the order - earlier-declared floats get HIGHER FPRs (same as GPRs); then route a sum through a value-returning inline ('dot = Dot3(nx, ny, nz, d)') to fix the last temp/const pair (CHearingVolume::HandleBulletCollision 975735ac, 15 -> 0).
- (lines) A float fabs result coloured wrong (frsp into a freed reg vs orig's older reg): call a value-returning 'inline float Abs(float x) { return fabs(x); }' DIRECTLY in the if-condition instead of a named float local / (float)fabs cast (CPlane::GetIntersection ae41f603, 50 -> 0).
- (soldier) A bool param passed straight on to a bool param with a lone 'clrlwi rN,rN,24' (re-mask, no !=0 normalisation): pass it through a static inline whose param is 'const bool&' (SetMovementPath 46f4a1bc). By-value bool/int/char inline params give either nothing or clrlwi+neg/or/srwi.
- [bigfiles 10-27] A CSE temp (e.g. 'right - left' reused by several later expressions) in the wrong CALLEE reg vs a neighbour: make it a NAMED local hoisted to the function top and used at every reuse site ('span = right - left; midX = left + (span >> 1); ... span / 4'), then hill-climb its declaration position (CPlayerObject::Draw 31 -> 0 with a statement swap of the corner conversions, d2fcb0b3).
- (lines) Inline ctor folding '&member_list->m_head' into 'this+20' where the orig keeps 'addi r3,this,4; addi r6,r3,16' (list ptr reused for a later member call): take the address through 'list* self = this; m_head.m_prev = &self->m_head;' inside the inline ctor (CWorldVolume ctor e6461006). Also: port_unit's extab size can be short when the orig's cleanup actions continue (check the linked_size).
- [bigfiles 10-27] One FPR holding a param-like value, then its product, then the sum (fmuls f9,f9,x; fadds f9,f9,y): the source updated the VARIABLE in place ('peek *= r.z; peek += base.z;'), reusing it after its last other use; write the sibling axes as 'ox = p * r.x; ... ox += base.x;' too (PeekLeftRight 41 -> 0, 9de36d54).
- (lines) Strength-reduced loop with TWO offset IVs that step together (orig 'addi r3,-48' and 'addi r8,-48', one from the counter's init and one from another value): two counters in the source, each used in one index, e.g. 'int j = n; for (int i = n; i > 0;) { --j; a[--i] = x; y = &a[j]; }' - one counter used twice gives one IV, 'a[i - 1]' is not strength-reduced at all (__sinit_collision_cpp).
- (lines) A __sinit_<file>_cpp is named after the SOURCE basename: to land a file's static initialiser, the unit holding it must be src/game/<originalfile>.cpp - rename the adjacent fragment unit (config json + project.json entry + source + docs row), as alarm.cpp/alarm_builtins did.
- (soldier) Read the ORIGINAL extab before fighting an object-address register: if a stack object's EH actions are only DESTROYMEMBER/DESTROYBASE on a callee reg (r29 = this) and no DESTROYLOCAL, the calls between construction and destruction happen INSIDE the (inline) constructor body - move them into the ctor (BIFunc_CreateImpact 9b50be09: fixed the 'addi r5,r29,16' vs folded r1+64 and the extab at once). Weak vtable whose kept copy is another file's: port with --weak-duplicate '__vt__...' (port_unit adds stripped_sections .data).
- (lines) Loop counter + strength-reduced offset initialised as 'li r31,0; mr r30,r31' (offset a COPY of the counter) and a loop-invariant base in the next reg: the loop lives in an inline member taking the invariant as a parameter ('void Relocate(int base) { ...; for (int i = 0; ...) }', CStaticAnimMesh::Init 6d571ed3). Function-level loops give 'li; li'.
- [bigfiles 10-27] Vtable/base-list .data order that per-class emission can't give (orig: basesA, basesB, vtB, vtA) and calls that must NOT inline: compile the unit '-inline deferred' (WITHOUT auto) and write the source in REVERSE image order - deferred emission reverses the text back, the reversed definition order gives the data order, and no-auto keeps every call (IObserver/ISubject dtors 5afaff4a; replaced a #pragma dont_inline).
- (lines) Same 'loop inside an inline member with params' trick fixed BSUtilObjectInstanceMemoryAllocator::Init (4a56a79a: link reusing the base reg). Try it FIRST on any loop whose counter/pointer/offset registers are rotated.
- (parks) Lerp FPR colouring x/z swapped (orig base.x f5 / base.z f3, ours reverse) in 't*(b-a)+a' per component: read each component of the base INTERLEAVED with its own store ('float x = a.x; d.x = t*(b.x - x) + x; float y = a.y; d.y = ...') in an inline member Lerp(a, b, t); decl blocks / expression orders don't do it (AnimMotCalcAnimPos 5acc9000, also fixes AnimMotCalcFloatingBone's lerps).
- (parks) FUNCTION ORDER in a multi-function unit (-inline auto): a function that calls a CLASS's inline members is held back until the next FILE-SCOPE inline function definition (or the end of the file; held-back ones then come out in reverse). Fix the order by placing a file-scope static inline (one the next function uses, e.g. an 8.24 FixedMul) between them. Check with nm -n before port_unit ('Inconsistent address for .text' is the symptom). animmot_rot 5acc9000/05206c45.
- (soldier) Param-walking loop whose callee regs rotate (count/paths/i): walk a LOCAL COPY of the pointer param declared AFTER the counter: 'int i; T* p = param; for (i = 0; i < n; i++, p++)' (CreateSplinePathManager cc4ec83b, 10 -> 0; 'param++', '&param[i]', copy declared before i all rotate).
- [bigfiles 10-27] Dead trailing 'b X' blocks before the epilogue (X = an inline loop's condition block) = out-of-line split critical-edge blocks: the loop var was the incoming local/param itself, not a copy ('while (head->m_next) head = head->m_next;' instead of 'p = head; while (p->m_next) ...') - CParticleSystemManager::Draw 3 -> 0. Also seen in for(;;){...return} loops (MemoryCardGC QuerySlot).
- (parks) Register of a struct pointer local vs plain array indexing: when the orig never forms a channel pointer, write every access as 'channels[index].field' (CW CSEs it) - a 'channel = &channels[index]' local changed the volatile colouring (AnimMotCalcFloatingBone, permuter find). Conversely a pointer PARAM walked with ++ sat in the lowest callee reg; '&channels[i]' inside the loop gave the orig r31 (CreateWeightList).
- (bigfiles) Virtual call at the start of a guarded block scheduled 'lwz r12,vt(this); mr r3,this' where the orig has 'mr r3' first: move the guard INTO the static inline as an early return ('if (!(cond)) return;') - UpdateSystem abb13382. A local only the inline needs but whose FPR must be low: declare it in the caller, pass by reference.
- (soldier) Two loops whose 'same-role' locals share ONE callee reg in the orig (soldier r26 in both) while ours rotate: the second loop's inline helper should take the caller's variables BY REFERENCE ('CSoldierObject*& soldier, AIObjectView*& target' declared in the caller) instead of declaring its own (CheckEnemies 773ad5f5, 12 -> 0).
- (lines) Big function workflow that worked (PatchUpAllPropertyData 2620 B, d0edc14c): Ghidra decompile -> draft -> fix struct diffs (case-body order, inline depth for stack slots) -> knob hill-climb (scratch/par/lines/chr.py, {{a||b}} templates, 0.3 s/compile) to find which statement shapes move colouring -> minimise junk knobs one by one. Inline helper for an inner loop (EndianSwap(PropPlane4&)) fixed a callee+volatile rotation two loops away.
- (extabpad) EXTAB PADDING PATCH (user-approved 2026-10-27, tooling c27aa295): code matches but 'Compiled source bytes differ: extab' only in the 2 padding bytes of a new/new[] cleanup (0x8b DELETEPOINTERCOND: action+6, ours 066c) or of the catch(...) block MWCC wraps around a class-operator-new (0x90 CATCHBLOCK_32: action+2, ours ff7f); original is 2500. Do NOT fight it in source: add an entry to config/GMFE69/extab_patches.json {unit, function (mangled), offset (hex, from the start of the function's extab record), action "0x8b"/"0x90", compiled, original} and a row in the docs/Progress.md "Extab padding patches" table (helper: python3 scratch/par/extabpad/addpatch.py <unit> <func> <off> <compiled> <original> <action>, under the lock). Find the offset: python3 scratch/par/extabpad/origx.py <func addr> <n> (original record) vs scratch/par/extabpad/dumpx.py <tmpl> <src> (ours); research check: scratch/par/extabpad/trypatch.py <tmpl> <src> <func> <off> <compiled> <original> <action>. The build fails if the bytes are anything else or the offset is not such a field, so the patch never hides a source change. Tag the landing commit "[extab-pad]". scratch/one_unit.py applies the list too.
- (lines) 'Scheduling-window' anomalies in long runs of identical inline swaps (one swap done sequentially instead of interleaved) = some members are a SUB-STRUCT swapped through its own inline (vec3 via EndianSwap(v.x, true) without the float& local) - group fields into 3-float vectors until it matches (BPDLight/BPDLightVolume swaps).
- (soldier) Callee FPRs of one switch case wrong while another case is right: the cases may SHARE a function-level float - hoist one float (e.g. 'float u;' used by case 9 for wp->x) and reuse it in the other case for a different component (case 2 offset.y), plus split the CVector3 into floats ('float x; float z = 0.0f;'). Brute-force which hoisted var each component uses (GetWayPointInfo 3171463d, wpi12/v4.py). Also: '-(double)v.x' vs '-v.x' changed scheduling, and brute-force independent statement orders (23 -> 4).
- 2026-10-27 (coordinator): land.sh now ABORTS (before commit) if progress_report --capture fails; it used to commit without the snapshot. On 'PROGRESS CAPTURE FAILED': re-run python3 tools/reconstruct.py, then land.sh again.
- (parks) SELF-RECURSION inlined to the wrong depth (whole extra level, +50 instrs) after an otherwise-right rewrite: CW's inline budget counts statements/nodes, so a '(void)0;' (approved stand-in) or other neutral statement in the inlined callee restores the original depth. Constants folded across inline levels ('depth + 4 + strlen' -> depth+5 hoisted) need the constant adjacent to the param in the source expression (DebugProfTimerDumpRecurse 40c53133).
- (parks) Library-like code: look for an SDK/demo reference in src/dolsdk2001 (e.g. DEMOPuts.c for RFPuts 655308ba) before drafting from Ghidra.
- [bigfiles 10-27] Old drafts with many 'li rX,0' where the orig has 'lis/addi' = globals declared with small class views (<= 8 bytes -> SDA21). Give every big global its real size from nm -S (Gameloop_Init rd 148 -> 84 in one step).
- [bigfiles 10-27] Store sequences relative to a member sub-object (stb 322(r25) with r25 = p+2000) = the setters are members of the sub-object's class (CCamera::SetNear), not of the outer class; call them through the member/a static inline taking the sub-object pointer.
- [bigfiles 10-27] An early 'mr rA,rB' copy of a pointer that a later loop strength-reduces = a second pointer variable assigned from the same expression ('info = mp = &g_Shell.m_mp;' with both declared first); 'info = mp' copies coalesce.
- [bigfiles 10-27] .bss-base functions next to a file's sinit: land the WHOLE file as one unit (mainloop.cpp 6885d700 folded mainloop_loop): delete the fragment's json/src + project.json line, port the sinit unit's id with the full source.
- [bigfiles 10-27] decomp-permuter on a C++ draft: autoperm needs this-> on members, no ctor-style temporaries (wrap in a static inline MakeVec), no references in locals (pointers), typedef for nested types (typedef CSkyBox::SVertex SkyVertex), loop counters declared outside 'for' (pycparser DeclList assert); then patch <dir>/fixup.py line 20 with the (?!\s*[.\[]) lookahead. Found the skybox Draw colouring in < 2 min.
- [bigfiles 10-27] .rodata/.sdata2 item ORDER that no statement order fixes (aggregate templates numbered before strings/constants of EARLIER functions): the file was compiled '-inline deferred' -> templates are numbered at parse time in source order, constants/strings at codegen in emission order; write the functions in REVERSE image order with '-inline deferred' (Pause.cpp 2c7e2559). Check the orig @N numbers vs text order first.
- 2026-10-29 soldier: trick 'swap the ROLE of a reused function-level variable': two loop temps colour-swapped (r26/r27) where one is a function-level var reused in later loops -> make the function-level var play the other role (e.g. the 'next' temp) and use a fresh local for the walker (ComputeAStarPath 1c473c69).
- [parks 10-29] Unreachable branch after a return ('b epi; b top' latch of a for(;;) whose switch cases all return/continue): a DEAD test of a non-const global around an empty inline call ('default: if (!MPDquiet_flag) Report(code); break;') keeps the loop's back-edge block; CW removes the test only after block layout. Const flag / empty stmt / no default lose it. (Get_Hdr 8a951390)
- [parks 10-29] CW branchless signed compare 'x < y' -> t = x ^ y; r = (t >> 1) - (t & y); result = r >>> 31. The 'and' operand is y (the larger side). If ours ANDs with the other operand, the draft's relation is reversed (semantic bug, e.g. iSNDserve 'pos > middle'), not a colouring problem. Also: an add operand-order diff in front of it may just follow from that.
TRICK [bigfiles 10-29] Partial whole-file unit (Render..end of projectilebullet.cpp, 1384be73): compile the merged draft with fast.compile_object and compare objdump -h section sizes (.rodata/.data/.sdata/.bss/.sbss) with the original file record ranges BEFORE porting - if sizes match exactly, port_unit places and owns them. Vtables need RTTI on + the real single-inheritance chain from the RTTI base lists (.data @NNN lists, all offsets 0) + every overriding class's slot declared, so each vtable reloc names the original symbol (check: objdump -r -j .data vs scratch/vtdump.py).
- [parks 10-29] A compare chain whose every branch lands on the same block (no loads in the arms) = DEAD computation whose loads were removed but whose control flow stayed: e.g. the high half of a 64-bit value truncated on the store (BIG_locateentryz: 'value = small(p, n); value <<= 32; ... value |= lo; *offset = value;'). Same family: dead 'if (!global) empty_inline();' keeps a block (Get_Hdr).
- [parks 10-29] Several file-local tables addressed from ONE base register with folded displacements (lbz 292(rX), lwzu 804(rY)) in the orig, but ours materialises base+292 etc. in registers: define the tables in the unit (own the ...data.0 block) AND try compiler 1.3 instead of 1.3.2 (port_unit --compiler 1.3). readsamples b0ae2f9b. Quick check: copy the template json with compiler_version '1.3' and score.
- (soldier) Volatile-reg swap between a loop's strength-reduced offset and an outer iterator (orig: iterator lower reg): write the offset IV out as a byte offset 'int offset; int i; for (i = 0, offset = 0; i < n; i++, offset += 4) (*(T**)((char*)arr + offset))' - i initialised BEFORE offset (else the two li's swap). EnableAll e7f6430e (11 -> 0).
- [bigfiles 10-29] Byte compares the orig does with cmplwi but WITHOUT merging adjacent constants into a range test (cmplwi 32; cmplwi 33): cast each side explicitly '(unsigned)*p == ' '' (unsigned char/unsigned int variables make CW merge ' '/'!' into (u8)(t-32)<=1; plain int compares give cmpwi). CTexture::Set palette knot e9025359. Also: a computed bool passed to a GXBool (u8) param without the byte mask: 'cond ? GX_TRUE : GX_FALSE' (srwi only). And types.h pulls libc stdio/string in C++ linkage: wrap '#include <stdio.h> <string.h> <dolphin/gx.h>' in extern "C" { } when calling printf/strncmp.
- [parks 10-29] Small struct copied by WORD (lwz/stw via a stack temp filled with stb/stb/sth): declare it with INT bitfields ('int a : 8; unsigned int b : 8; int c : 16;') and fill the fields one by one; a char/short struct or an initializer scalarises the copy. (PATH_createtrackfrombuf dd647642; same as PATHRECORDVIEW in PATH_event notes.)
- [parks 10-29] A constant n=1 in an unrolled search loop materialised as 'mr rN,rK' (copy of the 1 used as shift base) instead of 'li rN,1': put the search loop in its own inline returning the index.
- [parks 10-29] port_unit 'Inconsistent address for .text' = the object's function order differs from the original: a function that calls inlines is held back until the next file-scope inline definition - move an inline definition right after it.
- (soldier) A virtual call's slot load ('lwz r12,N(r12)') scheduled too EARLY: don't hoist the object into a local - keep the last pointer hop inside the call expression ('AIDoodadView* doodad = obj->GetAIDoodad(); doodad->filter->Virtual(args)' instead of 'CAIFilterObject* filter = ...->filter; filter->Virtual(args)'). AISetupTargetMatchList 7 -> 0.
- (lines 10-29) FPR SPILL SET wrong in a big function (orig spills x, ours y; same uses): CW picks spills by use count, ties by live range,
  and the live ranges follow SOURCE statement order inside each block - reorder independent statements that feed the same call
  ('ra = ...; rb = ...; proj = ...;' instead of proj first) until the spilled variables match (box-box 5a8cfe1c, struct 91 -> 11).
  Then colouring followed definition order once temporaries in switch cases were block-scoped ('case 7: { float qy = ...; }').
- (lines 10-29) Functions > 6000 B were never in untried_now.txt (untried.py 150 6000): run 'python3 scratch/uiw/untried.py 6000 99999'.
- [bigfiles 10-29] FPR colouring between two groups of temps (struct-copy lfd pair vs fctiwz conversions): temps CREATED later get LOWER FPRs, so
  brute-force the statement order of the block (all permutations, ~300 compiles at 0.3 s); if moving a use after a struct copy breaks a CSE'd
  load (struct copies kill memory CSE), read that value into a named local BEFORE the test that loads it ('float red = c[0]; if (red != 0 || ...)')
  (CMSHMatBin::Link b9369c22, 12 -> 0).
- [parks 10-29] Several loads feeding a call, loaded in REVERSE field order in the orig (last argument first): pass the field reads directly as the call's arguments (CW evaluates arguments right to left) instead of loading them into locals first. (SNDPLATFORM_downloadcomplete f3bb368a)
- [parks 10-29] An inline parameter that one branch overwrites (slot = alloc()) makes CW copy it at the inline's entry; the orig copying only in the other branch = a separate local ('index = slot;' in the else).
- [bigfiles 10-29] autoperm puts EVERY definition before perm_fn in prefix.cpp: a weak/out-of-line member that the unit defines AFTER the target
  (e.g. a copy ctor) gets auto-inlined there -> huge base score (DrawNode 6645 vs 80). Move it to <dir>/suffix.cpp and append it in compile.sh
  after the fixup output. Also kill permuters by the 'timeout' PID from ps|grep -v grep (pgrep -f on a string in your own command line kills your shell).
- [parks 10-29] 'bne next; b found' inside a search loop (instead of 'beq found') = 'return index;' from a static inline search function (the return becomes a jump to the inline's exit). AnimChanStartAnim 31dac232 (struct 4 -> 0). Also: a loop entered with 'b cond' whose condition re-loads a member (no break) = a plain top-tested 'while (m < 0 && !flag)' rather than 'while (!flag) { ...; if (!(m < 0)) break; }' (AnimSeqGrow 1c7c999e).
- (soldier) Out-param of an inlined finder ('Find(..., &entry)') colours with an extra 'mr' (iterator not coalesced with the caller's
  var): move the call into a small inline helper that holds the whole if-body (entry a helper local) - CW then coalesces it and sinks
  the param load below the branch. A value the orig keeps in a high volatile reg across an inlined loop (eventNumber r11) can be a
  repeated field expression ('record->m_handler->eventNumber' twice, CSE'd) rather than a local (bsevents ProcessEvents 32 -> 7).
- (soldier 10-29) Volatile swap INSIDE an inlined callee that already matches standalone (state r9 / CSE'd flags r10 vs orig reverse): rewrite
  the callee with standalone-equivalent forms and score callee + caller together (soldier/bpe16/ft.py). 'stateId = states[stateId]->parent;'
  instead of 'stateId = state->parent;' (same code standalone) fixed both inlined copies (BSEventProcessEvents/Immediate e4e34d00, 7 -> 0).
- [parks 10-29] FRAME SIZE off by 8/16 with identical code: check local ARRAYS whose elements CW promoted to registers - it still reserves the declared array size in the frame. Try smaller/larger dimensions ('char* src[2]' not [3] for a per-channel array: SNDDRV_fillbufwithpackets c3242479, frame 96 -> 80).
- (soldier 10-29) '...rodata.0' blocks (question (e) skips): almost every game file's block STARTS with the header's static
  dwi::dwi_prime_list[27] (108 B: 0x35, 0x61, ...), then the file's strings/RTTI names/const tables in definition order. When a function's
  strings come right after the primes, define the prime list + the function in its neighbour unit and port with '--pool .rodata'
  (AllocateGenerateBuffers 3d1e56c9, first compile). Survey: scratch/par/soldier/bpe16/rodata1.py <list> (list from rodata0.py) prints each
  blocked function's block base and whether it starts with the primes; dump base+108.. to see what precedes the function's strings.
- [parks 10-29] 'not rX,m; and rY,rX,v' (instead of andc) = the AND mixes types: '(unsigned int)ptr + ... & ~intmask' (a conversion between the not and the and stops the andc fold). MEM_allocaligna 1bb51814.
- [parks 10-29] After the structure matches, a clean callee-register PERMUTATION of function-level locals often follows declaration order directly: list the orig register of each local (r31 downwards) and declare them in that order (MEM_allocaligna: 65 -> 0 in one step). Worth trying before any hill-climb.
- [parks 10-29] Reassociated adds ('a + (b + 16)' where the orig does '(a + b) + 16'): permute the source terms, e.g. 'needed + 16 + alignmask' gives (needed + alignmask) + 16 (CW moves constants last).
- [lines 10-29] UISStackProcess (8236 B bytecode VM): an empty 'case X: break;' whose jump-table entry the orig points at a lone 'b'
  (ours retargets to the switch exit) = a no-op statement CW drops LATE, e.g. a cross-member union self-assign 'sp[-1].p = (void*)sp[-1].i;'
  (plain self-assigns, dead stores, empty inlines, if(0), goto, continue all get retargeted early).
- [lines 10-29] Volatile colouring in a huge switch: C-style declarations at the top of the case (vreg order = declaration order)
  flipped many r3/r4/r5 swaps; also declaring the second temp before the first in a 2-byte reader inline.
  'x + data' operand order: pop into a local first, then an inline 'Addr(screen, off) { return base + off; }'.
- (bigfiles) Rotated integer registers across several loops in one function: give each loop its OWN pointer local (even when
  one variable could be reused) and brute-force the declaration order of all locals hoisted to the function top
  (CStaticMesh::DrawNode 10 -> 4: 'int j; int i; unsigned long clipped; MSHMaterialView* material; MSHMaterialView* m2;
  MSHItemView* item;', smd4/gen.py tries all 720 orders).
- (bigfiles) Deferred-inline files with RTTI: a class's RTTI type-name string lands BEFORE all function strings in .rodata
  when its key function is parsed after its base class's key function, otherwise AFTER all of them (scr/e1-e3.cpp).
  Orphan class-name strings in a file's string block = RTTI names of linker-stripped classes (Screen.cpp).
- (soldier 10-29) GetObjInfo (6 KB switch) 101 -> 16: (1) a callee-saved local loaded from a script-stack arg ranked too low: read it
  through a value-returning static inline ('target = ArgObject(stack, count, 2)') - it moved above the shared shift CSE temp;
  (2) FPR names of a 2-term dot (orig accumulates in place: 'fmuls fA,fB,fA' into the first operand's reg) =
  'float dot = d.x; dot *= f.x; dot += d.y * f.y;' (also 'dot = dx; dot *= f.x; float dy = ...; dot += dy * f.y');
  (3) the orig's zero constant sharing a callee reg with a pointer that is 0 on one path = 'iresult = (int)(filter = 0);' at the top
  plus 'else filter = 0;' at one site (CW then CSEs the later 'iresult = 0' stores to the same register).
- (lines 10-29) Callee-register ORDER of params/locals wrong after decl-order sweeps: brute-force the integer TYPES (unsigned long/unsigned int/long/int) of params and locals - CW orders callee regs differently per type. extern "C" functions have free param types (no mangling). UISProcessEvent d1ad5980: 'long event' 16 -> 9, then 'i' before 'end' -> 0 (upe/ty.py).
- [parks 10-29] 'addi rT,base@l; ...; mr rV,rT' (address constant into a temp, then copied) where the orig does 'addi rV,base@l' directly
  = the orig's loop-carried pointer is a STRENGTH-REDUCED INDEX ('int k = 0; p = &arr[k]; ... k++'), not a user pointer initialised
  with '&arr' (CW always materialises a constant initial value of a multi-def variable through a temp). SNDMIXI_fxadd struct 9 -> 0.
- (bigfiles) Two consecutive loops in one (inline) function whose second loop's IV/temps are rotated: declare ONE shared index 'I i;' at the top of the function and use it for both loops ('for (i = last - 1; ...)', 'for (i = first + 2; ...)') instead of a 'for (I i = ...)' per loop - the second IV then reuses the dead param's register (dwi::quick_sort_ insertion_sort inline: 22 -> 6).
- (lines 10-29) one_unit 'Compiled source bytes differ' when the scorer says 0: objdump the linked scratch/one/<id>/compiled.elf vs orig and diff - IDENTICAL switch-case bodies laid out in the wrong order only show as branch targets (reorder the cases). Also: pointer walkers 'p'/'q' in the orig may be CW strength-reduced IVs of 'text[i]' (no pointer variables in source): the IV increments come right after the counter's, in loop order (UISStringFormat).
- (lines 10-29) Callee-register order of PARAMS that are passed on to calls: the CALLEE's prototype param types matter (int vs unsigned long) - IStudio::ISirenStartup 26 lines -> 0 by declaring extern "C" UISGetMemSize/UISInit with unsigned long params (isu/ks.py: random knob search over all prototype/member/local int types).
- (soldier) '...rodata.0' block whose strings sit AFTER RTTI class names / other files' objects: the file may be DEFERRED-INLINE.
  With '-inline deferred,auto' CW creates string constants at codegen time (after the end-of-TU vtable/RTTI emission), so .rodata =
  parse-time objects (header dwi_prime_list, aggregate initialisers of local tables) + RTTI names + strings, all in one unit. Hint: the
  strings' @N numbers are HIGHER than the RTTI names' (nm the orig). Soldier_object.cpp: merged 7 landed fragments + FireWeapon into one
  deferred+RTTI unit (source reverse text order; the class's first-declared virtual defined in the unit = vtable/RTTI emitted; declare
  overrides so every vtable slot names the original) and it owned .rodata/.data/.sdata exactly (44a2a210). Check other blocked files.
- [parks 10-29] "No reference" runtime ASM functions (ExPPC_LongJump, __throw) are cheap: write CW 'asm' functions straight from the
  disassembly (nofralloc, or fralloc + a struct local and 'la r3, local'; fralloc homes non-register params itself) - raw 0 first try.
  Exception-runtime C++ (__unexpected): '#pragma exception_magic' + '__exception_magic' compile under GC; strings 4-aligned = drop
  'pool' from -str. port_unit may STRIP strings only reached through the ...rodata.0 base (stripped_objects) - check the .rodata size.
- (soldier 10-29, b69a8cef) ENUM LOCAL BLOCKS IF->ARITHMETIC: CW rewrites 'if (c) x = K; else x = K+1;' (int/long/short/char) into cntlzw/neg/addi
  arithmetic. Declaring x with an enum type keeps the branches (li hoisted + bne + li). The two-def branchy form also changed callee-saved
  register order (the def-inside-branch variable drops to the lowest callee reg). Try on any register rotation where a local takes one of two constants.
  Candidates (uncovered fns whose orig keeps an adjacent-constant if/else as branches; scan: python3 scratch/par/soldier/adjscan.py <objdump -d of the ELF>):
  AnimStProcess 0x800672c4 (li r27,3/2; parks 'asp'), MemoryCardAutoLoad 0x800e42e8 (r31 65/66), RestartGame 0x80019e54 (2/3),
  CScreenFieldMode::_Capture 0x8001b13c (5/4), CCompartment::Init 0x800761c8 + CStaticMesh::Init 0x800f25bc (9/8), MohDVDErrorTask (1/2),
  TRKSuppAccessFile (208/209), MemoryCardBootUpPortCheck (128/127). 0/1 cases are listed too but are mostly bools.
- (bigfiles 10-29, f7ac0ed7 CWorldObject::Acquire) INLINE PARAM COPIES ARE LATE VREGS: a value passed into an inline helper (end pointer,
  'this', node) becomes a fresh copy created late -> it gets a LOW register (r3.., even r0 if never dereferenced). When the original
  keeps such a pointer in a higher volatile register across several loops (r7), write the helper's loop in place against the named
  local. Conversely, passing an existing local into an inline (PushRequest(request, end)) makes CW reuse it (CSE across loops only
  works for the same named local, never for re-computed '&m_list' expressions).
- [parks 10-29] PARAM REGISTER SWAP (two params' callee regs swapped vs orig, MEM_resize 37b3189f): copy one param into a typed local
  right after the decls ('p = (char*)address;') and use the local everywhere (incl. the return); then rerun the decl permutation search.
- [parks 10-29] LOOP COUNTER BELOW ITS IVs (orig counter in a higher callee reg than the strength-reduced pointer IVs; decl order inert):
  move the loop into a 'static inline' helper with its own 'int i' (and any function statics it uses) - CheckDVDError 87a98bff exact.
- (lines s16) A by-value aggregate copy (e.g. a 4-row box frame) whose stack slot lands ABOVE the caller's named locals while the orig puts it
  BELOW all of them: copy it as separate NAMED per-row locals inside the inline ('CVector3 r0 = box.m_frame.m_row[0]; ... r3') - fixed every
  slot of TestCDBBranchNode<CVolBox> (76 -> 53). A struct copy or an array keeps the wrong (size-class) placement.
- (bigfiles 10-29, 41b619ec MemoryCardGC) SWITCH CASE-TREE SHAPE: CW builds a segment tree after merging empty case targets; labels whose
  bodies are all 'break' merge with default gaps (orig -2/-4 nodes became one '-3' node). To keep a label a separate target, give it a body
  that CW drops only AFTER tree building: a dead self-assignment with a cast ('result = (int)result;', approved class 10-23) - placed as
  the LAST case so no leftover 'b' is emitted. Dead stores/empty inlines/empty ifs/gotos vanish before the tree (no effect). Extra labels
  can be needed too (here 'case -1' in the break group). Search tool: bigfiles/mc/pr/rnd.py (random label sets x target groups on small
  probe functions, prints exact compare-sequence hits; compile 2000 functions per object).
- (bigfiles 10-29) '#pragma dont_inline on/reset' around a callee's DEFINITION stops deferred auto-inlining of it (around the caller does
  nothing in deferred mode). Also lines' tk.py fixed a whole function's callee registers + frame via one 'unsigned long' local.
- [parks 10-29] SKIPPED VOLATILE REGISTER (orig uses r5 where ours uses r4, nothing visible in r4): the orig had a local that was
  optimised away but still took the register - e.g. an inline helper with 'int* counts = timer->counts;' (DebugProfTimerInitAll 15162799).
- (lines s16, 59930c8b) Stack slot order vs copy schedule are separable: DECLARE the copy locals uninitialised in slot order
  ('CVector3 r0, r1, r2;'), then ASSIGN them in the order the original schedules the copies (here reversed). Init-declarations tie both together.
- (soldier 10-29, e8cf43ee Moh2.cpp whole file) Deferred-file merges: (1) static functions the orig CALLS but deferred auto-inlines in ours
  (single-call statics, small wrappers of inlined helpers) -> '#pragma dont_inline on/reset' around each callee's definition. (2) fast.py
  normalises SDA constant references, so a wrong float literal (0.1f vs 0.0f) scores 0 - port_unit's relocation pairing catches it
  ('Inconsistent address for .sdata2'); compare pool items by name before porting. (3) Array of class objects initialised from
  temporaries that the orig copies field-by-field + operator= = the member class has a user copy ctor '{ *this = m; }' (CW then keeps the
  temp). (4) port_unit now keeps numbered objects found at the same original address+size in every section (strings reached via
  ...rodata.0+offset; base lists reached only through weak tables) - before, any --weak-duplicate made it strip them.
  Harness: scratch/par/soldier/m2/sc.py (per-function, -s = register-blind diff), cmpdata.py (all data sections vs the orig record).
- (parks) A DEAD duplicate compare ('cmpwi r3,0' immediately followed by another 'cmpwi r3,0; bne') after a call = the trailing
  'if (err != 0) break;' of a ONE-ITERATION loop ('for (i = 0; i < 1; i++) { ... }') that CW flattened (TRKMessageSend a81c5868).
- (parks) u8 accumulator truncated after EVERY add ('add; clrlwi' per unrolled step) + unrolled base 'addi t,i,16; add t,p,t' =
  'sum = (u8)(sum + buf->data[j])' with a SIGNED 'int j' (an unsigned index folds the +16 into displacements and defers the truncation).
- (parks) Library code hiding as NOTYPE size-0 labels (asm files) never shows up in function lists: scan for code bytes not under any
  named FUNC (s17 found TRK __exception.s = 7988 B). CW 'static asm' wrapper + 'opword' per word + 'entry <label>' reproduces the bytes
  and the size-0 global labels exactly; port_unit currently cannot keep it (see parks notes QUESTION).
- (bigfiles 10-29, 671c9ca4 DrawViewportEdge) LATE-VREG RULE, NAMED-LOCAL FORM: a value read once into a named local ('int index =
  player->m_index;') is an EARLY vreg and takes the higher register; the original's lower register came from NOT naming it: pass the
  expression itself ('IsTop(player->m_index)', CW CSEs the loads into one late temp). Try deleting a named local whenever two volatile
  registers are swapped between a named value and a global/CSE load. ALSO: fast.py masks float constants - always run poolvals before
  trusting a fast 0 (the old DVE draft had +3/-2 where the orig has +5/-4).
- (bigfiles 10-29, CInputManager::Update 135 -> 5 lines) ONE loop counter declared once ('unsigned long j;') for several clear loops makes
  CW CSE the stored zero with the counter's init (orig 'li r12,0; mr r10,r12; ... mr r3,r12' = per-unroll copies); a 'unsigned char raw'
  set in each branch then 'int value = raw;' reproduces a join 'clrlwi'; reading a field through the base ('table->keys[index].flags')
  BEFORE taking 'key = &table->keys[index]' swapped the key/flags callee registers into place.
- (soldier 10-29, staticobjfactory WIP) Bool-returning inline predicates ('bool IsAnimated()' used in '?:') materialise 'li 1; clrlwi.;
  beq' - write the condition inline in each inline getter ('if (idx >= 0 && (idx & 2)) return x; return 0;'). A pool pop whose orig loads
  the free-list head straight into a callee reg: test the GLOBAL first ('if (!g_pList) {msg; return 0;} slot = g_pList;'); the form
  'slot = g_pList; if (!slot)' loads into r6 and copies. A placement-new target copied to a callee reg only AFTER the null check =
  a second typed local ('CTankObject* mem = (CTankObject*)slot; new (mem) CTankObject(...)'). A loop where the orig keeps a SEPARATE IV
  starting at a constant ('li r30,56' + 'addi rX,r30,4' at use) = the offset written as one grouped sub-expression '(i * 24 + 0x38)'.
- (parks) Field loads the orig issues ALL before the first store/arith of a small computation (GCSW_VD ctor centring, 7 -> 0, 77ad7de5;
  MOHFL_VD_create 45 -> 17): read every input field into plain int locals first ('int w = h->width; int fw = t->fbWidth; ...'), then
  write the stores as single expressions 'm_x = (fw - w) / 2;'. Locals computed then stored ('int x = ...; m_x = x / 2;') did NOT work.
- (soldier) CW colours top-level 'new T[n]' (T with ctor+dtor) result pointers (EH DELETEPOINTERCOND temps) with the HIGHEST callee regs, before loop webs; one inside an if-block gets a low reg and shares with a later counter. Decl order, scope blocks {}, inline helpers around the news do not change it (staticobjfactory Init, probes in soldier/s22/p*.cpp).
- (soldier s24, CheckFieldOfViewModeList 2 -> 0, s24/CFOVML_BEST.cpp) MEMBER LOAD SCHEDULED BEFORE THE 'mr rN,r3' THIS-COPY
  (orig 'lwz r29,1688(r3); mr r28,r3; lwz r30,global'): read the member into a local in the method ('start = m_listObject') and
  move the rest into a STATIC non-member inline taking 'CAIObject* self' (member inlines never copy 'this'). Then fix the
  remaining callee swap with the late-vreg rule: pass the global UNNAMED through a getter at the call
  ('Search(this, GetListStart(), start ? start : GetListStart())', helper param order (self, first, object)).
  Every named-'first' variant toggled between right order/wrong regs and wrong order/right regs.
- [parks 10-29] CALLEE-REG ORDER = vreg creation order (params, source vars in inline-expansion order, loop-opt IVs, then CSE
  temps LOWEST). A value the orig keeps in the lowest callee reg (shared with a later result) = a CSE temp, not a variable: read the
  field into a short local and write the sum at each use ('unsigned short len = b->length; p = Alloc(len + 4); memcpy(p, b, len + 4);'
  CopyStateDef c76e80d8). Same family: 'length = finish - start; length += lead;' (UploadFramesToARAM).
- [parks 10-29] Two loop CURSORS (from/to) swapped: copy each list entry through a small inline and try its PARAM ORDER
  ('CopyStateAnim(from, to)' vs (to, from)) - decl order/void*/types were inert, param order fixed it.
- [parks 10-29] Inline byte-swap temps (ChangeEndian bodies) get stack slots in BFS inline-expansion order (all level-1 calls in code
  order, then level 2 ...), words/shorts in separate pools: use the slot sequence to infer which swaps go through wrapper overloads.
- [parks 10-29] 'li i,0; mr iv,i' at a FUNCTION-level loop = the loop counter is REUSED from an earlier loop ('int i;' shared);
  a fresh 'for (int i' gives 'li; li'. Index param type of a helper (long vs int/unsigned) decides IV merging and IV order.
- [parks 10-29] CW won't inline a callee whose EXPANDED size exceeds ~6-7 ChangeEndian(int) bodies (any inline keyword/flags);
  a weak header inline that is too big is emitted out-of-line at the END of the file (EndianSwap(AnimFileFormat_t&)).
- (bigfiles 10-29) fmuls/fnmsubs operand order: 'expr / 2.0f' gives 'fmuls fD,expr,half' while both 'expr * 0.5f' and
  '0.5f * expr' give 'fmuls fD,half,expr' (CW canonicalises the multiply but keeps the divide's operand order when it turns it into *0.5).
- (bigfiles 10-29) A global read BEFORE a call in the orig (lfs before bctrl, value kept in a callee-saved FPR) = the field read into
  a float local in a statement before the call ('float l = g_viewport.left; l *= screen->vf28();'); 'g.left * call()' loads after.
- (bigfiles 10-29) Local aggregate with non-constant members ('VertData v[4] = {{0,0,L,T,..}}') = rodata template copy + stores of
  the variables, emitted AFTER the values are computed - a match for 'copy loop after the calls, then stfs into the copy'.
- (bigfiles 10-29) An unreferenced function-static in a .data block (e.g. buffer$836 between two used statics) can be reproduced by an
  unused 'static char buf[N] = "";' in a function at the right source position: CW still emits it.
- [parks 10-29] Volatile pointer/byte-temp colour swap inside an inline byte-swap (ptr r5 vs r4): reach the ChangeEndian body through
  ONE MORE typed wrapper (cast to unsigned int&/unsigned short&, long -> unsigned int -> int, template T*& -> unsigned int&); shift
  every field by the same level so the BFS stack-slot order stays (AnimFileRelocate f221eb2e). A guard test that sits inside an
  inline helper may belong in the caller ('if (d->magic == X) Helper(d)') - it changed the param's colour.
- (parks 10-29 s20) Volatile FPR rotation around a scaled-and-squared param ('float d = 0.5f * r; d *= d;' gives the 0.5*r temp the
  FIRST free FPR, orig gives it the LAST): name the intermediate - 'float half = 0.5f * r; float d = half * half;' (ChooseCoverPoint
  FPRs exact). A permuter result like 'x = x;' (self-assign junk) after a compound assignment points at this.
- (parks 10-29 s20) Toy finding (scratch/par/parks/s20/toy/batch.py): with no other pressure, CW gives callee-saved GPRs in vreg order,
  LATER vreg -> HIGHER reg (params left to right, then locals by first def); a source copy ('A* self = this;', inline param binding)
  coalesces into the EARLIER vreg and changes nothing. So an orig where 'this' outranks a later local is NOT reachable by copies.
- (parks 10-29 s20) Register-order levers differ by pressure: in small functions callee GPRs follow vreg creation order (later ->
  higher; propagated copies/inline param copies change nothing). In big loop bodies the order follows spill cost: removing one call
  or adding a pointer kept across a call flips two callee regs (AnimStProcess owner/slot). Ablate statements one at a time and print
  the param/local regs (parks/s20/asp/abl.py, ccp/abl.py) to find WHICH construct sets the order before trying source forms.
- (lines 10-29 s19) Sum of an SROA'd vector temp and a vector read from memory: the inline helper's OPERAND ORDER matters for colouring
  even though CW canonicalises the fadds operands: tcap contact point 'SetSum(point, sv, m_vertices[0])' (temp first) gave the orig's
  base-load registers (28 -> 25) while 'SetSum(point, m_vertices[0], sv)' did not. Try both orders of every SetSum/+= on SROA'd temps.
- (lines 10-29 s19) FPR toy (lines/s19/toy/gen.py): with decl order == def order, named floats colour first-fit f0..f13 then f31
  downward in declaration order; with defs in reverse declaration order the pattern breaks (not a pure decl- or def-order rule).
- [bigfiles 10-29] A call-defined local of a static INLINE ('dump = HitDumpObject(...)' inside PickUp) always took the LOWEST callee reg
  (r25, below the params) whatever the declaration order; one extra graph node live across that region fixes it, after which inline
  locals rank by declaration order (LATER-declared = HIGHER reg, the reverse of function-level locals). Code-invisible ways to add the
  node: pass the switch value BY VALUE to its own inline ('Classify(self, other, crc, crc, ...)' with 'int value' for switch/first use,
  'const int& crc' for the later stack reloads), or an int->long / int->unsigned by-value inline param. (player OnCollision a4bed99e)
- [bigfiles 10-29] 'this' stuck in the lowest callee reg (params rank below locals): a 'CAIObject* const self = this;' local declared
  FIRST (the const matters: a plain 'CAIObject* self = this;' is coalesced away and changes nothing) gave this r31; then an 'int t =
  type;' enum->int copy and the declaration order (self, from, t) placed the other two; the call through a static inline
  Select(self, position, from, int type, ...); FPR temps fixed by top-declared 'float z, y, x, distance;' + 'float half = 0.5f *
  radius; distance = half * half;'. ChooseCoverPoint 23 -> 0 (bigfiles/ccp/CCP_BEST0.cpp).
- [bigfiles 10-29] *** PARAM-RANK FIX: 'T* const self = this;' (or 'const' copies of other params) declared FIRST in the function and
  used at a couple of sites. A plain 'T* self = this;' is coalesced away; the 'const' local survives as its own top-ranked node and
  takes the param's colour with it. MemoryCardLoadGame 64 -> 0 with that ONE line (this r28 -> r30 above slot/file), ChooseCoverPoint
  23 -> 0. Try it on every 'params rank below locals / this in the lowest callee reg' near-miss (MNFIT, RegisterTargetCheck,
  CreateDestructable, ISirenStartup, CreateSplinePathManager, death_cb ...).
- (bigfiles 2026-10-29) CW '-inline auto/deferred' will NOT auto-inline a function whose body constructs a local object of a class
  with any user-declared ctor (even an empty inline default ctor); calls to other inline helpers inside do not block it. So if the
  original auto-inlines a function with vector locals, those vector classes are PODs; get pair copies from a user operator= ('V a; a = b;')
  and ctor-temp register order from a 3-float inline overload. Whole-file merges: AIObject 61f98ea8 (scratch/par/bigfiles/aio/build.py
  = generic head + per-unit renames + chunks re-sorted by original address for deferred order; views.py = CW offsetof probe of views).
- [bigfiles 10-30] A member RELOADED in the orig right after an earlier load of the same field (no store between; ours forwards it):
  read the earlier use through a LOCAL REFERENCE ('const V& rate = m_rate; ... targetRate.x - rate.x' while 'm_rate.x += c' stays
  direct) - CW CSEs loads per base expression, the reference is a different base vreg (UpdateTargetPosition struct 53 -> 8).
  Also: inline loop temps hoisted into one 'float a, b, ...;' at loop top + coefficients read into named locals (ka = m_a) moved
  them from callee to volatile FPRs; ut/so.py = dependency-safe statement-order + decl-order hill-climb, ut/tidy.py restores natural order.
- [solo 11-01] Tools: scratch/solo/pred/varmap.py <tmpl> <draft> <func> <start> <end> uses '-sym on' DWARF (code identical on 1095 units) to print each NAMED variable's register and the register the original wants. Read it before guessing.
- [solo 11-01] Measured on landed code: callee-saved locals, EARLIER declared = HIGHER register in 85-91% of GPR pairs; params usually below locals. A local assigned from a value-returning inline ignores its declaration position; write the expression directly and it ranks normally (PATHI_choosebeat).
- [solo 11-01] A const named copy can flip a scheduling tie: 'int const wanted = count;' (Decode), 'UISScreen_t* const scr = screen;' (UISExecuteFnc). Usually inert (2 wins in ~1600 sweep variants), but cheap to sweep: scratch/solo/sweep/copies2.py.
- [solo 11-04] Named copies that "get copy-propagated away" do survive if the LATER USES go through them too: 'int const a = opts.x; int const b = opts.y; sum = a + b; ... gs->x = a; gs->y = b;' fixed a two-register swap of CSE'd field loads (SNDPLATFORM_outputset 5 -> 0, order a,b; 'int' not the field type). Try on any swapped pair of re-read fields.
- (loop systems) '...bss.0' BASE-REGISTER functions are NOT blocked any more: define the file's .bss block objects in the fragment in original order with exact sizes (size views; NO constructors, so no __sinit is compiled; globals fine), then port_unit '--pool .bss' (+ '--pool .sdata2' etc.). NOBITS pool items compare as zeros. Landed this way: CParticleSystemManager::Draw fbe4be84, Reset/InitTimeStampStuff (0 on first compile; old best 31/60 were all base-addressing). Candidates found by scanning orig relocs for '...bss.0': bsbifunc built-ins (BSInitBIFuncs, AddPlayerAmmo, DisplayPromptText/Integer, DisplayHintText, BSResetBIFuncs, displayPromptBuffer, BSCleanUp/UpdateBIFuncs), objcreate BSGO_Destructable::Destroy, DestroyBuddyDataStructures, CCompartment::Draw/Init/InitClass, CStaticMesh::Draw/InitClass, CParticleSystem::RenderSystem. Block layout: list orig symbols from the '...bss.0' address up (the first object sits at base+0).
- (loop systems) Adjustor thunks '@N@f': CW emits them (reverse table-slot order, after __sinit/weak copies) only in the TU defining the class's key function = FIRST non-inline virtual DECLARED. Declare a leaf virtual first (IsUsed), define it with its real body, '--file-duplicate' it and '__vt__X'. f12a2e09.
- (loop scripting) Values converted from script args (degrees->radians etc.): read every arg into a local first, THEN reassign each in place ('x = 3.1415927f * x / 180.0f;'), then build the vectors from the locals. Conversion inside ctor args or initialisers left 23-59 colouring lines that 720 order permutations never fixed (RotateRel 13057d9d).
- (loop scripting) Two adjacent stores to different globals come out in the opposite order to the source when the colouring needs it: try swapping the statements (BSResetBIFuncs: marker store before 'g_iHighestObjectiveNumber = 0' + pointer-walk clear loop for the shared zero, 44 -> 0).
- (loop scripting) .bss trick for template containers (buddy.cpp): the containers' implicit ctors would compile an __sinit; drop the ctors from the fragment's views of list/list_node/vector/BuddyInfo (templates instantiate only used members) - Destroy then compiles with no initialiser. Trailing unreferenced block objects can be left out.
- (loop scripting) In big hand-written fragments with an else-branch that only uses fresh locals: declaring those locals ('int z; int y; int x;') at the TOP of the function, in reverse order, fixed the tail colouring (CreateDestructable 33 -> 11).
- (geometry) Written-out dot products stuck on b0/b1 product order: write them as inline Dot() calls and brute-force the statement order (GetNearestPoint 13 -> 0: b0, a01, a00, b1, a11). The inline call changes the schedule ties that 120 orders of expanded dots never reached.
- (loop systems 2) '...rodata.0' blocks whose HEAD belongs to other functions: compile those functions (already landed elsewhere) in the fragment as '--file-duplicate' so their strings/templates/RTTI names precede yours, then '--pool .rodata'. Check the file's inlining mode first: in a '-inline auto,deferred' file aggregate templates are numbered at PARSE (they come first in .rodata), string literals at codegen in TEXT order (= reverse source order), and RTTI type names are created when the first function using a class's table is compiled. debug_prof 9de067ca: DebugStats_Draw (template), CProfHistRenderBin dtor + Render (RTTI names; '-RTTI on') as file duplicates -> VerifyNew/DumpAll offsets exact. port_unit gaps (fix the manifest by hand, then one_unit): file-local globals used only by duplicate code land in undefined_in_discarded_code -> move them to externals + local_externals (+ sda_externals); duplicates WITH extab records make port_unit mis-slice extab/extabindex -> set address/size to your functions' records and linked_offset/linked_size to skip the duplicates' (compiled order = text order).
- (loop systems 2) Literal position probe (toilet.cpp): a file-scope 'static const char* const' or 'static const char x[]' literal is created at parse (before every function's strings); a const POINTER is NOT folded (loads from .sdata), so it is never the original form for a lis/addi '@N' reference.
- (loop airender) Pooled .bss blocks with global objects that have destructors: the original block holds a 12-byte destructor-chain record (@NNN) before each such object; define a 12-byte size view there so later objects keep their offsets (staticmesh_initclass, compartment_initclass). Check offsets with objdump -t before porting.
- (loop airender) Index/value register swap in a gather loop: the original re-reads the field inside the test ('if (p->f >= 0) { int v = p->f; ... }'), CSE'd away (HandleExplosion 53 -> 0; found by the permuter, autoperm needs C-parsable decls: typedefs + an inline Init instead of ctor args).
- (loop airender) A struct copied through a by-value return (GetColor()) gives the orig's word temp at 8(r1) then byte loads (EndDraw 19 -> 11).
- (loop airender 2) .bss blocks holding CMatrix/CVector3 objects: view them as plain float arrays (MatrixView float m[16] aligned 16) and cast at the uses ('((CMatrix&)g_M).Multiply(a, b)', '(CVector3&)g_CamPos = local'). Even an empty inline 'CVector3() {}' makes a file-level object compile a 4-byte __sinit; a class view with NO declared constructor (CCompartment g_GlobalCpt) is fine. CStaticMesh::Draw / CCompartment::Draw.
- (loop airender 2) Virtual call via 'lwz r12,0(r3); lwz r12,N(r12)': a function-pointer cast through a vtable member loads into r5 etc.; declare a real virtual at slot N ((N-8)/4 placeholder virtuals before it).
- (loop airender 2) An unreferenced weak function compiled BETWEEN kept functions (template dtor) can be listed --linker-stripped since 5ef1c047 (port_unit used to keep its moved exception records when a same-size record follows the unit in the original).
- (loop scripting 3) INLINED appendMessageToPrompt (DisplayPromptText/Integer, AddPlayerAmmo): write the chunk as 'strncpy(g_promptCursor, text, end - text); g_promptCursor[end - text] = 0;' (no 'length = end - text'): the strlen length then shares a callee reg with 'end' as in the orig and the whole displayPromptBuffer rotation falls into place (Text 20 -> 0, Integer 15 -> 1 in scratch/loop3/scripting/pt/b3.cpp). The landed appendMessageToPrompt still matches with this form. bsbifunc.cpp is DEFERRED (helpers sit after their inlined callers in text); with '-inline auto' define appendMessageToPrompt/displayPromptBuffer before the caller and port them as --file-duplicate.
- (loop scripting 3) STACK SLOT ORDER of inline byte-swap temps (ChangeEndian arrays): deeper inline nesting moves a temp later in the frame order. BSInitBIFuncs: header field swapped via 'template ChangeEndian(T&) -> ChangeEndian(unsigned int&) -> ChangeEndian(int&)' and the entry swaps in a 'SwapEntry(BIFEntry&)' helper put every slot in place (225 -> only the .rodata-base offsets left).
- (loop systems3) RTTI type names at the HEAD of a deferred file's ...rodata.0 (after the primes), derived name before base name
  (objcreate: BSGO_Player, BSGO_Basic): compile the derived class's key function (landed elsewhere) as --file-duplicate and make
  the BASE's virtuals in-class inline ('virtual void Destroy() {}'). An out-of-line '__declspec(weak)' base virtual makes the
  base its own key-function owner: its name comes first and the derived name lands AFTER all strings. Also file-duplicate the
  derived __vt__ and both __RTTI__ records (externals + local_externals, NOT undefined_in_discarded_code), slice extab/extabindex
  (linked_offset/linked_size), and drop port_unit's 'adapted_from' when --like is a scratch file (test_provenance).
  NonVisDestructFarthestSoldiers, objcreate_nonvis. objcreate's later .rodata.0 users (CreateGerman @1549) can reuse it.
- (geometry) 2D cross products written as a tiny inline 'Cross2(ax, ay, bx, by) { return ax * by - bx * ay; }' called with the difference EXPRESSIONS as arguments (no difference locals) fixed CSE/fusion shapes: _find_aim_blend_values 94 -> 5 real lines. Also: a random decl-order x assignment-order search (split 'float a, b, ...;' then assignments) took CVolBox::GetExtents 60 -> 45 where axis-order tries had stalled.
- (loop airender 3, CCompartment::Init 578 -> 2 lines) (a) An auto-inlined member that stops inlining at its 2nd call site after small
  edits: declare it 'inline' explicitly (larger budget). (b) Orig 'bne L; b M; L: li rX,0' (CW folds ours to one beq): give the
  then-branch a real but redundant statement, 'if (t == 0x32) palette = (T*)((char*)header + off); else palette = 0;' - this also
  un-rotated every callee reg. (c) Constant-select if/else that the orig keeps as a real diamond ('beq; li 4; b; li 8'): write it as
  'switch (x & 1) { default: s = 4; break; case 1: s = 8; break; }' (every if/ternary/type/enum/bitfield form became a select).
  (d) Byte-command writers whose value load is scheduled before the stores: evaluate the value first ('{ unsigned v = value; p[0] = 8;
  ...= v; }') or pass it as an inline param. (e) Command pointer 'p' must rank low (callee reg) and the stride high (r0): p as an
  inline param ('WriteVCD(list, p)'), the stride computed in the CALLER and passed as a param copy ('BuildDL(list, stride)').
  (f) Loop index IV inits 'li rA,0; li rB,0' (ours 'li; mr'): use a fresh single-def loop variable ('for (int n = 0; ...)').
- (geometry 2) Tool: scratch/loop3/geometry/fab2/blk.py = per-basic-block (split at blr) diff counts vs the orig; lets a search keep only variants that fix ONE case of a multi-return function even when they break others (to learn which source construct drives that case). Also: no Collision/Animation queue function references a ...bss.0/...rodata.0/...data.0 block, so the layout recipes do not apply to that lane.
- (loop mid1) *** Rising Sun's ORIGINAL binary (../moh-rising-sun/orig/GR8E69/MOH3RDVD.ELF, ProDG, inlines much less) keeps out-of-line
  copies of helpers that Frontline inlines. `objdump -t` it for a member/static near your function (or disassemble RS's copy of
  your function and list its 'bl' targets): the helper's name, signature and body shape are the original source structure.
  RotateFromScript/TranslateFromScript (parked 24/4 since 10-15 on FPR colouring) = 0 on the first compile once the time code
  became RS's inline member 'float CStaticObject::CalculateMovementTime(const CVector3& rate, const CVector3& distance)' with two
  ternaries (70aad029). Colouring knots that no decl order fixes are often a wrong helper boundary.
- (loop scripting 4) Mirrored FPRs (x/z swapped) in an inlined helper that matches in another caller: the INLINE NESTING DEPTH decides. Route the call through one more inline level in the caller that needs it, using a natural form: an overload taking the container iterator by reference ('GiveBuddyOrders(dwi::vec_iter<T>& it) { GiveBuddyOrders(it.operator->()); }'), an updater taking 'vec_iter<T>& it', or a dump body taking the container as a parameter ('DumpAttached() { DumpVector(g_list); }'). A plain pointer parameter or the same code written out gives the other colouring. Then a 'T* const' parameter fixed a GPR coalescing (buddy UpdateBuddyProgress 112 -> 0, BuddyDied 16 -> 0, GetInitialBuddyOrders 33 -> 7).
- (loop scripting 4) .rodata-base chain in a file whose block head is the dwi header's prime table: MERGE the next function into the existing head unit (define 'static const unsigned long dwi_prime_list[27]' in the dwi namespace, keep the globals extern, pool .rodata + .sdata2) and fold the small weak-template units in between (delete their json/src + project.json lines; CW emits inline template members right after their first user, so the text order is the original's). Point drafts that used the deleted jsons as compile settings at the merged one (index.tsv).
- (loop airender 11-11) Raw/masked register sharing ('lwz rA; clrlwi rA,rA,2' vs ours 'lwz rB; clrlwi rA,rB,2'): mask through a POINTER local 'T* q = s->f; q = (T*)((unsigned long)q & 0x3fffffff);' - every integer temp form failed (CCompartment::Init 2 -> 0). Permuter harness for C++ bodies: macros for placement new / reference casts + real C structs in PERM_PRETEND (scratch/loop3/airender/pci).
- (loop systems4) objcreate creators (CreateGerman exact, CCV): (a) 'new T' through a CLASS 'static void* operator new(unsigned long)
  { return alloc.GetFreeElement(true); }' keeps the ctor stores on the saved reg ('mr. r30,r3; stw ..,(r30)'); global placement
  new stores through r3. (b) 'mr r0,r3; mr. rX,r0; beq; bl __ct; mr rX,r3' = 'p = (T*)alloc.GetFreeElement(true); p = new (p) T;'.
  (c) 'fmuls f1,conv,const' with a power-of-two scale = '(float)s / 16.0f' (CW turns it into a multiply; '* 0.0625f' puts the
  const first). (d) A guarded function-local static int ('lbz init$; extsb.; ... stb') is just 'static int n = 0;'.
  (e) A dead 'if (w)' block placed right AFTER the then-branch's call, whose path skips the rest: an inline member whose
  else-branch ends with 'return;' (CSoldierObject::SelectFirstWeapon). (f) A vector temp in the slot of an earlier local plus a
  second stfd copy: the code REUSES that local ('position.Set(0,0,1); aim->SetDirection(position)' with a by-value inline param).
  (g) A store pair reloading the base once per pair: setter inlines taking two floats.
- (loop airender 5) Rising Sun helper-boundary check came up EMPTY for the AI/render near-misses: RS's renderer is EAGL (fonts,
  draw contexts, scene draw), its cover points are TriggerObject-field based, and its collision response is CClsnConstraintsBase, so
  those Frontline functions have no RS counterpart. Where RS did show a helper Frontline inlines (SExclusionEntry ordering ctor,
  UNodePair ctor, list iterators in SweepExtents; RemoveWeapons; GetDirectionVector), CW compiled our spelled-out form and the helper
  form to identical code. Check `objdump -t` of the RS file for the same class layout first; skip when the subsystem was rewritten.
- (loop scripting 5) RS helper boundary, 2nd win: CreateWeaponByCRC 17 -> 0 (47e3ee8e). RS calls GetWeaponPropertiesByCRC out of line;
  Frontline inlines that WHOLE function (CRC remap switch that reassigns the parameter + record search) where the old draft had
  invented Remap()/FindProperties() pieces. The callee is defined AFTER its caller and is global, so the file is deferred-inline:
  the fragment defines the callee first (body copied from its landed unit) and ports it as --file-duplicate.
  Scan tool: scratch/loop3/scripting/rs/rsscan.py "<queue area>" lists, for each open row, RS's copy and its callees that have no
  Frontline symbol (= helpers Frontline inlined). Tried with no gain on: UpdateProximityTriggerStatus (UpdateProximityTriggerState
  + CheckFullTree: 13), CreateDestructable (AddObjectToList/AllocateBoxVolume/GetBasis/GetCenter: 11), CThrownObject::Init
  (GetBasis 16, GetCenter worse), TriggerEnterAndLeaveMessages (RS Enter/Leave + SpecialCase/UniqueProperty: worse - extra inline
  levels make CW stop inlining in a 3.5 KB function). RS has no buddy.cpp (buddy system rewritten), no CPublisher.
- (loop rs1) Survey tool for the Rising Sun trick: scratch/loop3/rs1/cmp.py <FL mangled names> prints FL bl targets and, for every RS
  function with the same name+class, the RS bl targets FL lacks (= helpers RS keeps out of line). rsbl.sh <RS sym> [-d] lists/disassembles.
  Results: geometry rows have NO lead (RS anim is EALA, RS collision draws the same helpers we do); most Audio/Shell RS code is a later
  library version (PATH, SND, PopUp), so check the helper body matches FL semantics before using it.
  CUserMessageTable::Load 10 -> 0 (95aae29e): RS CreateUMStructFromFile as an inline member that RE-READS the members it needs (m_index,
  m_text) fixed the shared-zero shape; passing them as params stops inlining. The new boundary re-rolled the loop colouring (14); then
  hoisting the loop's locals to the TOP of the helper and sweeping their order (120) hit 0. After a boundary change, redo the decl sweep.
  PATH_event 26 -> 6: RS PATHI_getmastertrack inline, called AFTER the struct-field inits that precede it in RS.
- (loop scripting 6) A separate strength-reduced IV with an extra '+4' added in the loop ('li r30,56' ... 'addi r6,r30,4; lwzx') comes from
  a pointer-cast index '((const char**)&fields[i*3+7])[1]'; member spellings (fields[i*3+7].s, nested structs, union arrays) fold the
  +4 into the shared IV (CStaticObjectFactory::Init variant loop). Also: CStaticObjectFactory::Init's EH new[] colouring is inert to
  decls, helpers, types, scopes and 2.9k permuter iterations (2nd time); don't retry without a new idea about EH temp live ranges.
- (loop rs2) Two products sharing a byte factor with swapped registers (beats r10/r11 vs eventnum*beats): hold the byte in an INT local
  ('beats = map->beats', unsigned char local stays wrong) and give the first product its own local built in two steps ('a = eventnum;
  a *= beats;' - permuter find; 'a = eventnum * beats' keeps the other operand order). PATH_event 6 -> 0.
- (loop rs2) CW '#pragma dont_inline' is read at the END of a function definition: around a callee's definition it stops BOTH the callee
  being auto-inlined AND the callee's own inlines (PATH_pause lost selectstate). Around the caller or on a prototype it does nothing in
  '-inline auto'. '#pragma auto_inline off/reset' around the callee blocks only the first (not on the accepted list yet: asked).
- (loop systems5) Callee-reg order of a POINTER READ FROM A PARAMETER (core = trigger->core) vs the params: declare the
  local FIRST but ASSIGN it after const copies of the params ('T* core; int const c_param = param; P* const c_trigger =
  trigger; int z; int y; int x; core = c_trigger->core;'); read the field directly, not through an accessor inline
  (GetCore() kept it low). CreateCollisionVolume 78 -> 0 (5c8d9fb5). A placement-new result variable ranks just below
  the first new-temp whatever its declaration position; only a converted copy ('(T*)new ...') follows declaration order
  (but adds a cmplwi/mr pair) - CreateDestructable stays at 3.
- (loop mid2) Placement-new RESULT ranking (objcreate CreateRootHierObject 44 -> 2): 'p = new (alloc.GetFreeElement(true)) T;'
  always gives p a HIGH callee reg whatever the declaration order; 'p = (T*)alloc.GetFreeElement(true); new (p) T;' makes p
  an ordinary local ranked by declaration order (then brute-force the top decls: scratch/loop3/mid2/gen3.py). Cost: the inline
  ctor stores go through p instead of r3. Worth trying on CreateDestructable's box/param swap.
- (loop scripting 7) Loop increment ORDER: orig 'addi i,1' before 'addi p,4' = 'for (i = 0; i < n; i++, p++) f(*p);' (body 'f(*p); p++;' gives p first). TriggerEnterAndLeaveMessages -28.
  int->bool copy 'mr r5,rX' before a branch = 'bool b = (bool)x;' / implicit 'bool b = x;' ('x != 0' computes straight into the arg reg).
  Access-path CSE: 'g_arr[i].f' and 'p->f' (p = &g_arr[i]) are NOT CSE'd with each other; mix them per site with a random search
  (scratch/loop3/scripting/tel/et4.py) - TEL 152 -> 116. Tool scratch/loop3/scripting/tools/sdiff.py = structural diff with difflib alignment.
- (loop pnew1) Placement-new FUSION rule (small tests in scratch/loop3/pnew1/t*.cpp): the cast form
  'p = (T*)new (...) T' / 'p = (T*)AllocX()' gives the orig shape ('mr. rP,r3; beq; stores via r3', p ranked by
  declaration) ONLY IF p is dead before the next conditional branch; if p is live across any later 'if' the copy
  becomes 'cmplwi r3,0 ... mr rP,r3' after the join. The direct form 'p = new (...) T' always fuses, but p becomes
  the new-expression temp and ranks above all locals. CW always null-checks placement new (any operator new form),
  and keeps the redundant beq after 'if (p) new (mem) T'. CreateRootHierObject/CreateDestructable stay parked at 1.
- (loop late1) The CCV late-assign recipe did NOT transfer to CreateDestructable / GetInitialBuddyOrders / CSoldierObject::Draw /
  CStaticObjectFactory::Init / UpdateProximityTriggerStatus (inert or worse). What did help: (a) const POINTER copies of REFERENCE
  params ('list<T>* const c_ext = &extents;' declared right after the first local, uses through them) moved a pointer read from the
  param into its orig rank (SweepExtents); (b) an inline helper's LOOP written out in the caller fixed a whole volatile-register
  block (SweepExtents Pairs: 31 -> 7 real); (c) 'bool const flag = **stack;' fixed the flag/stack callee swap in
  BIFunc_AIStartCoverPointWalkToStored but merged the conversion into the callee reg (orig converts into r0 + mr): 3 diff lines.
  (d) A 'T* const x = new ...' declared at its use ranks LOWEST (by declaration order), unlike a non-const new result (ranks top).
- (loop sweep1) fmuls operand order (orig 'conv * var' with the product landing in var's own callee FPR): a value-returning inline whose body is a compound copy 'inline float Mul(float a, float b) { float r = a; r *= b; return r; }' called as 'Mul((float)screen->GetWidth(), viewWidth)'. A function-level 'w = conv; w *= vw;' gets the order right but takes a NEW callee FPR; '(float)x * v', 'v * (float)x', '+v', const copies all canonicalise to (v, conv). DrawTextCenteredF 3 -> 0 (2e4ac3cd). The second product there wanted the plain function-level compound 'c = (float)w; c *= viewX;'.
- (loop sweep1) 'and'/'or' operand order inside small bitset operators: 'int t = m_bits; t &= o.m_bits; m_bits = t;' flipped the and operands where 'm_bits &= x', 'm_bits = m_bits & x' and 'x & m_bits' all gave the other order (FlagAsOccupied 4 -> 3, scratch/loop3/sweep1/FAO_BEST3.cpp).
- (loop scripting 8) Unfolded 'addi rX,rB,4; lwz rX,0(rX)' (CW folds every pointer/ref/inline/volatile spelling of the load) plus an
  aggregate-init template copied through 4 distinct registers: make the aggregate local CONST ('const SAcquireSet set = {...}') - one
  change gave both, TriggerEnterAndLeaveMessages 108 -> 0. Also: a bit test '(x & 0x8000) >> 15' (vs '(x >> 15) & 1') keeps rlwinm.
  but changes the scheduling of neighbouring inits (mr/clrlwi order).
- (struct1) A '...rodata.0' base-register user can be code-exact and still unlandable: check the orig base offsets against the file's orphan block items (scratch/tmp/block_orphans.py) BEFORE investing. Play/UserInterface::Init both blocked this way.
- (struct1) Stack aggregate whose first word is an int read/written through memory and arrays indexed from base+4/+68 = a local STRUCT {int index; float a[16]; float b[16];} with inline ctor/Add, not separate arrays (OPTION_PARSING_PLAYER::Play, 523 -> 22 with a decl hill climb).
- (struct1) Double copy 'mr r29,r3; mr. r27,r29' after an allocator call = inline CLASS operator new (size from the class size: give the view its real size); single 'mr.' = global/placement new. Word copy after four stb = CColor(r,g,b,a) passed as a temporary.
- (struct1) frame.py (scratch/loop3/struct1): print a compiled frame size; delete statements one at a time to find which source construct allocates an extra stack slot.
- (loop late2) ITERATOR OBJECT for list walks: a raw 'list_node<T>* node' loop variable vs a 'list<T>::iterator node' (struct {list_node* m_node}
  with begin()/end()/operator*/operator++/operator!= by value, as Rising Sun include/game/DwiContainers.h list_iter) changes the
  variable's register rank. SweepExtents: node r30 vs the CSE'd m_data copy r31 (inert over 1.5k decl orders, const copies, data locals)
  -> exact with the iterator form, and late1's const param-copy workarounds became unnecessary (b9dfe832). Try it on any
  dwi::list walk with a node-pointer rank swap.
- (loop scripting 9) Negative: session 8's const-aggregate and '(x & m) >> n' tricks found no natural site in MechFollowPath,
  UpdateProximityTriggerStatus, DisplayHint, AddAmo, CStaticObject::OnCollision, PublishEvent, CreateDestructable or
  GetInitialBuddyOrders (const CVector3/SItem/BuddyInfo copies are optimised away with identical code). CW canonicalises
  int expression spellings completely: raw-vs-(+1) locals, ++/+=, inline Square(), const, late assignment all give
  byte-identical MechFollowPath code, so a missing callee reg there is not a spelling of that value. Details: scripting/notes.md.
- (loop struct2) MWCC 1.3.2 SCALAR-REPLACES ctor-less local structs (CVector3, float[3][3]) even when inline helpers take them
  by reference/pointer, use operator[], unions, base classes or copy ctors. It keeps a local in MEMORY (stores kept, even
  dead ones) only when a NAMED pointer/reference is bound to it ('T* p = &v;', 'const T& r = v;', 'T& t = *this;' inside
  an inline) or it has a destructor (which adds EH actions). An original whose vector locals are all stack-resident with
  no calls taking them therefore needs a pin form we don't know yet (RenderSystem, UpdateBoundingBox).
- (loop struct2) Stack slots of named locals: arrays and 16-aligned class objects (CMatrix) are placed first (highest
  addresses) in declaration order, then the other locals in declaration order (first declared = highest), then
  temporaries (by-value param copies). Ctor-less vectors can be hoisted to fix their order; keep CMatrix/CColor
  declarations where their constructors run.
- (loop struct2) switch: case bodies are emitted in SOURCE order and the compare tree falls straight into the body only
  when 'default:' is written first ('default, case 0, case 1' for the orig's default-first layout).
- (loop struct2) 'bool b = f(); b &= g;' keeps the AND result untruncated until the use (orig 'and r30,r3,r0' ... later
  'clrlwi r30,r30,24'); 'unsigned char b = f() & g' truncates at once, 'bool b = f() & g' adds neg/or/srwi.
- (scripting s10) CVector3 WITHOUT CONSTRUCTORS (aggregate; keep a pair-copy operator= if assignments need lfd/stfd): the implicit
  copy is still an lfd/stfd pair (16-byte aligned(8) struct), but it does NOT take the source's address, so CW treats the vector's
  fields like scalars in memory: it keeps loaded fields in callee FPRs across calls (orig f31/f30 + one field reloaded from the home
  slot), and it FUSES squares of them (fmadds x,x,yy) - a user copy ctor '*(Pair*)this = *(Pair*)&o' (or any ctor, even 'V() {}',
  which instead scalarizes the whole struct) blocks both. Symptom to look for: orig fmadds on 'x*x' from a stack vector, or a vector
  field kept in f31/f30 across a call while ours reloads. CImpact::Visit 598 -> 0 (176f30c6). Construct values field-wise.
- (loop sweep2) Value-returning arithmetic inlines move FPR colouring the way Mul does for products: 'inline float SubV(float a, float b) { return a - b; }' on ALL the min-block terms plus 'ax = ax < bx ? ax : bx;' selects made CVolCapsule::GetExtents' min block exact (9 -> 4; scratch/loop3/sweep2/cex/BEST4.cpp). Use them on whole groups: one wrapped term alone did less. A compound-body 'SubC' (r = a; r -= b) was worse.
- (loop sweep2) 'u * 1.05f' coming out as 'fmuls fN,const,u' while the orig does 'fmuls fU,fU,const' (in place, value first): 'd = Mul(u, 1.05f)' with the compound-copy Mul fixes both operand order and in-place reuse (CAnimObject::ConstrainVelocity 24 -> 16 with a statement-order hill climb, scratch/loop3/sweep2/cvel/). Vector-operator forms (CVector3 operator* built as a compound copy) are far worse there (180+, stack temps).
- (loop sweep2) Negative result: compound bodies inside a shared inline (Cross2, Dot, Length) break fmadds fusion everywhere (_find_aim_blend_values, DrawNode, CThrownObject::Init). Use Mul/SubV only at the call site of the product that is wrong.
- (loop sweep2) POD CVector3 lever checked on the geometry knots (ThrownObject::Init, DrawNode, _matrix_rot_yzx: inert; aim_cb 67 -> 255, box-sphere 20 -> 174): their originals copy vectors through the stack (lfd/stfd then lfs), so look for the register-kept/fused-x*x symptom BEFORE dropping constructors.
- (loop late3) Negative for late2's list-iterator trick: GetInitialBuddyOrders' DumpReadyBuddies and PublishEvent ALREADY use
  iterator objects (shape variants: by-value !=, default ctor, post-increment, for-init, while: inert). Struct-iterator wrappers on
  intrusive m_next lists (UpdateProximityTriggerStatus German loop) and the m_weapons[9] array (CSoldierObject::Draw): inert.
  The trick fixes a swap only where the draft walked a dwi::list with a raw list_node pointer; grep drafts for 'm_head.m_next'
  loops first (remaining: buddy chain + list ctor, all already blocked on other knots).
- (loop struct3) An array global read through a local pointer: if CW rematerialises the constant address (lis/addi
  again after a call) instead of keeping the pointer in a saved register, declare the array with the type other
  units use ('unsigned long g[20]') and copy it through a cast ('int* data = (int*)g;'): the cast copy is a real
  variable (PlayScriptedSound, 98 -> 86 + one saved GPR). Also: a file's own small non-inline function
  (GetAzimuth) is auto-inlined under 'inline auto'; its by-value parameter copy is a stack temporary, which can
  explain a 'swapped' pair of vector copy slots.
- (loop vec1) CThrownObject::Init 16 -> 0 (9b775aa0): the ctor-less CVector3 view only works TOGETHER with the plain
  Length body 'sqrtf(x * x + y * y + z * z)' (each change alone: 16 or worse). Then CW fuses the axis squares like the orig.
  Lesson: when a lever is inert, re-test it combined with a simplification of the inline bodies that touch the vector.
- (loop vec1) ALIAS-FREE PIN for stack-resident vectors: a member 'float& operator[](int i) { return ((float*)this)[i]; }'
  used ONCE on a local (even a read) makes CW keep that local in memory (stores kept, values still cached in FPRs) with no
  extra code. '(&x)[i]', 'd.v[i]', a Set/Ptr/Data member or an unused pair operator= do NOT pin (probes vec1/pp_*.cpp).
  The same for a 3x3 local through 'float* operator[](int i) { return ((float*)this) + 3 * i; }'. Replaces the declined
  'CVector3* pin = &v;' aliases: UpdateBoundingBox stays 101 alias-free (vec1/ubb_a.cpp), RenderSystem structural 943 -> 711
  (vec1/rs_m1.cpp; 644 with ctor-temp colour ranges + order climb, vec1/rs_ord_best.cpp).
- (loop vec2) Gram-Schmidt / 'a - b - c' vector sums: when the orig's second subtraction reuses the first result's register
  ('fsubs fA,up,r' then 'fsubs fA,fA,f'), write it IN PLACE: 'u.x = up.x - rx; ... u.x -= fx;' (one expression 'up.x - rx - fx',
  Set() or a ctor gave 21; in place 17), then Mul() on single products moved the rest (CMatrix::Orthonormalize 21 -> 7).
- (loop vec2) Box-corner / projection code where the orig copies a projected point out of a stack temp ('lfd 416; stfd 448'):
  project into a named 'point' and assign 'contact.m_vector1 = point' instead of projecting into the member directly
  (CVolBox::TestCollision(CPlane) 279 -> 154 rd, together with the ctor-less CVector3 view: 375 -> 279).
- (loop vec2) Symptom scan for the ctor-less view: _find_aim_blend_values, CQuaternion::Multiply, CVolBox::GetExtents are leaves
  without stack vectors; CMatrix::Rotate's f31 is the angle param; TestCollision(CVolCapsule) keeps copies in stack (view 145 -> 199).
- (loop scripting 11) DEAD stack copies in the orig (stores never read, CW normally deletes them): a copy is kept when its
  object's address is taken by a CAST ('*(Pair*)this = *(const Pair*)&o' in a user copy ctor), not by a named reference.
  Pair copy A then float copy B (lfs/stfs x,y,z of A) = by-value inline 'CVector3 Copy(CVector3 v) { return CVector3(v.x,v.y,v.z); }'
  with the result bound to 'const CVector3& r = Copy(src)' (and READ through r; unread refs/temps are deleted). DoTargetCheck 96 -> 61.
- (loop scripting 11) An extra 'mr rX,rCount' copy before a 2x-unrolled counted loop (count also used for 'offset += count * n'):
  'while (count-- > 0)' instead of 'for (k = 0; k < count; k++)' drops it and keeps the signed ble ('while (count--)' gives beq).
  BSFileReadClass 708 -> 414.
- (loop scripting 12) 'rlwinm r0,...; mr rResult,r0' after a float compare where the orig computes straight into the result
  register: check the function's RETURN TYPE. A bool function with a 'bool result' takes the compare result in place; an
  int result needs a conversion copy. Grep landed callers' declarations first (bsbifunc_schedules.cpp declared
  'bool DoTargetCheck'). A pointer assigned to the same bool ('result = GetScript(...)') keeps the orig's 'srwi r0; mr'.
- (loop scripting 12) Dead stack copies: CW 1.3.2 keeps float stores to an unread object when its CONSTRUCTOR writes through
  a cast of 'this' ('((float*)this)[i] = ...'); initializer-list, member, Set() and d.v[] bodies are deleted. Useful to
  diagnose; check the body against every landed unit using that class before trusting it (scratch/loop3/scripting/s12/viewcmp.py
  tmpl src old new -> IDENTICAL/DIFFERENT).
- (loop vec3) INLINE PARAMETER ORDER sets FPR colouring: UpdateBoundingBox's turning-point inline as 'Turn(float a, float v, ...)'
  (acceleration before velocity) fixed every v/a register swap in block 1 (93 -> 81); any permutation with a before v was equal.
  DECLARATION ORDER of separately declared floats also sets colours (first declared -> lowest volatile FPR), but only after the
  other knots are right (inert on the earlier draft). Try both before running a statement-order search.
- (loop vec3) A 'Min(x, y)' inline vs the written ternary 'x < y ? x : y' changes the load order (struct 2 -> 0 in UBB block 1).
  And 'x += k*y' through a Scale'd temp vector + operator+= gives the orig's fmuls+fadds (no fmadds) under fp_contract.
- (loop struct4) Loop over a member array where the orig keeps an OFFSET IV ('slwi rO,i,5' before the loop, 'addi rO,rO,-32' and
  'add r9,this,rO' each pass, the same offset reused by the exit-path copy) but ours folds 'this' into a pointer IV: index through a
  per-pass local 'int i = --low; m_grid[i].f(); ...m_grid[i].pos' (not 'low--; m_grid[low]', not 'int i = low - 1; low = i;' which
  is not strength-reduced at all). DetermineNewObjectivePosition 235 -> 95 (fc2de588). Also there: 'int low; int next;' declared
  before 'int line = FindLine(angle)' fixed an r3/r4 rotation of a whole inline search (95 -> 1).
- (loop struct4) A float struct copied FIELD-WISE into a stack local that the orig keeps in memory (lfs/stfs, reloaded after a call)
  while ours copies words (lwz/stw) or scalarises: member-wise assignments in the orig's store order + the operator[] pin used once on
  the local (GetClosestWaypointByType struct 17 -> 9). Load order follows from the store order; try all 6 orders.
- (loop struct4) When the orig shows ' bge X; bge X' (same CR, same target) inside a float && test, the source tests the same
  compare twice ('d < a[1] && d < a[1] && d < a[2]'); landed as a redundant test kept as in the original (DetermineNewObjectivePosition).
- (loop struct4) A file's other functions are inline candidates for its fragments: TLT_LoadFromRepository's body is the file's own
  TLT_LoadFileFromLevelBigFile (toilet_files.cpp) auto-inlined; copying that landed body gave the exact structure at once.
- (loop vec4) A plain value-returning 'inline float MulV(float a, float b) { return a * b; }' on EVERY field of a group of
  'v.x = k * axis.x' products (box half-axis vectors a, b, c written field-wise) gave the orig's ascending volatile colouring
  a=(f7,f8,f9) b=(f10..f12) c.x=f13 + c.y/c.z callee (TestCollision(CPlane) 154 -> 89 rd); the compound Mul on the same
  sites was far worse (366), Scale() or plain field-wise products 154. Try MulV on whole product groups, not single sites.
  Also: an inline member's FIELD ORDER (Negate body x,z,y) moves colouring of long-lived results (87 -> 85).
- (loop fresh1) fast.py/sbs.py normalise branch TARGETS to 'T', so a draft with wrong control flow can score 2 forever as a
  "register/IV order" near-miss. MoveStatsIntoProperAreaForIstudio (parked at 2 since 10-17, "IV increment order") was a wrong
  loop: the original advances the player index only when it copies ('if (active[window]) { memcpy(dest, &stats[i]); i++; }',
  window/dest every pass); fixing the logic also fixed the IV order (e959464b). Check: research/tools/brchk.py [max]
  compiles every index draft and compares branch offsets; scratch/loop3/fresh1/objchk.sh tmpl src func start end for one.
  2026-11-12 scan of all drafts <= 40 lines: only SFILTER_unpackxaf (length 42/43) and CheckForObstructions differ.
- (loop struct5) An inline search loop whose original does NOT constant-fold the starting index ('li r31,0; slwi rO,r31,1;
  cmpw r31,rN' where ours folds to 'li rO,0; cmpwi rN,0; ble'): start the caller at 'int index = -1;' and get the FIRST
  element through the 'Next' inline (index++ then Find) instead of 'index = 0; Find(...)'. GetClosestWaypointByType
  structural 9 -> 2 (struct5/gcw/GCW_BEST2.cpp). A local struct/array holding the index ('struct { int index; } it;')
  is a half-step (keeps cmpw, not the slwi).
- (loop struct5) Register-colouring diagnosis for an inlined helper: write the helper's body out in the caller once. If
  the caller form colours like the original, the knot is the inline's RETURN TEMP coalescing (TLT_LoadFromRepository:
  caller form exact registers, only 'bne +8; b' vs 'beq' left), so spelling changes inside the helper won't help.
- (loop vec5) A value that is a DECLARED local tends to overflow into a callee FPR before CSE temps and inline-internal
  values do (UpdateBoundingBox block 2: whichever of t / t2 is a function local takes f31). To push a value out of the
  callee set, make it an inline's PARAMETER fed an expression ('Kin(start, vx, vy, vz, sv, sa, (a - b) / 60.0f)', no
  local t) or an inline-internal local; to pull a CSE'd load INTO the callee set, give it a named local ('float svz;
  ... svz = sv.d.v[2];') and move its declaration (UBB 58 -> 56). 'const', a scoped block or a local passed to the inline
  do not help (all 72). Check with a who.py-style table (value -> register, ours vs orig) instead of the line count.
- (loop vec5) A scalarized CVector3 temporary (no address taken) never adds a stack slot: to explain a frame 16 bytes
  larger than ours, look for an address-taken (pinned) object, not a named temp.
- (loop fresh1) Old parameter-copy workarounds can now HURT: removing them gave death_cb 28 -> 5 (const c_header/c_event/c_data:
  the header param then ranks in order) and DMClusterSkelTraverseHeirarchy 143 -> 31 ('orient = orient_' style copies made CW
  split a later pointer into base + copy and add an instruction; length now exact). Re-test drafts' copies by deleting them.
  (PopUpMessageHandler::Loop's 'self' copy: inert.)
- (loop vec6) Callee/volatile swaps in a block of field-wise struct arithmetic: put an INDEPENDENT statement (here the Negate of
  the plane normal into the contact) and the remaining product lines as movable tokens in the order hill climb, not just the
  arithmetic. TestCollision(CPlane) 85 -> 56: finish corner.x (-= a.x, b.x, c.x), THEN Negate, then the c.z product, then
  corner.y/z. Earlier searches had held the Negate and the contact decl fixed and were all inert. Tool: vec6/tcp/hc.py.
- (loop struct6) 'li r3,0; beq join' (return value set BEFORE the branch) plus 'li r3,0; b join' after an exhausted search loop,
  inside an inline finder: hold the result in a local initialised to 0, 'return trigger' from the loop, re-zero it after
  the loop and return it: 'T* found = 0; if (all) { for (...) if (m) return t; found = 0; } return found;'
  (GetClosestWaypointByType structural 2 -> 0). 'if (!all) return 0;' gives bne/li/b; 'found = t; break;' keeps a mr.
  It also moved all four params from r24-r27 to r28-r31 (the colour regime of the whole function changed).
- (loop struct6) CW does NOT coalesce a copy between two USER variables in a search loop's found path ('it = p; goto found',
  any declaration order, also a hand-inlined 3-variable return temp): extra mr. Only an inline's return temp coalesces there.
- (loop vec7) Negative result worth knowing: when a callee/volatile swap between two values survives EVERY source form of one of
  them (TestCollision(CPlane): ~40 separation forms, function-level vs inline locals, helpers, decl orders all keep the same pair),
  stop varying that value; the pair is set by the other value's web (coalescing of a store-forwarded field). Use a who.py-style
  table (vec7/tcp/who.py: opcode-aligned orig>ours register pairs) to see at a glance whether the KEY pair moved, not the line count.
- (loop fresh2) INTERLEAVE COPIES WITH THEIR USES: two by-value vector copies whose fields feed a block ('a = start; b = end;
  then terms of a, terms of b') can colour the copy-load registers differently from the orig. Write 'copy a; terms of a; copy b;
  terms of b' (CVolCapsule::GetExtents 4 -> 0, 00b269c6; an 8-way interleave search found it in 70 compiles, cex/v2.py).
  Then RE-SCORE INSIDE THE TARGET UNIT: with the shared CVector3 header (cast copy ctor) the getters' return temps added an
  address CSE ('addi r6,r3,40'); copying members ('CVector3 a = m_axis.m_start;') gave 0 again.
- (loop fresh2) Before trusting a draft that inlines a helper, compare the helper's body with any LANDED out-of-line copy of
  it (grep src/game): CMatrix::Rotate's draft uses a hill-climbed BuildRot body that differs from the landed BuildRot (21 vs 31
  with the landed one); landing it would be the q8 per-fragment-view shape. Also check the draft's template json is the unit
  it would join (Rotate belongs at the end of the deferred-inline matrix_rotate, not matrix_translate).
- (loop fresh2) Compiler '@N' names of const-int& literal temps (.sdata) fingerprint the source order of their creation:
  gaps between them show other numbered entities (if/else ~3, '?:' 0). Compare orig 'nm -n' @numbers around the function's
  SDA loads with ours (scratch/loop3/fresh2/gpl/at.py, rel.py). They also fix the data order: a reordering that changes
  which temp is created first breaks the .sdata layout even when the code matches.
- (loop big1) Int->float->short angle blends ('lis 0x4330; xoris; stw; lfd; fsubs' x3 with rotated GPRs and the
  0x4330 store scheduled late): grid the SHAPE, not single knobs. UpdateAnimation conv block 39 -> 0 needed all of: short
  targets ('short dx = (int)(...) >> 8;'), the pointer local declared AFTER the targets, int copies of the old values,
  and per-component statements ('dx = dx - x; short sx = dx * t; angles[0] = sx + x;'); every single change alone was
  inert. scratch/loop3/big1/grid.py is a reusable itertools grid over a code block (0.5 s/compile).
- (loop big1) A value CSE'd from an inline called twice ('if (v.Dot(n) < 0) v += (v.Dot(n) * -2.0f) * n;' instead of a
  'd'/'k' local) gets a different FPR colour than a named local: k f9 -> orig f2 (UpdateAnimation 19 -> 16).
- (loop near1) A result COPY scheduled early ('mr r31,r3' right after the call, before other stores) with a later '+= remaining':
  write a running total initialised to 0 ('int total = 0; ... total += decoded;'), as sibling functions often do. CW folds
  0 + x into a copy that is NOT coalesced, so the natural update order works (SFILTER_unpackxaf 7 -> 0, landed 5d836f39).
  Look at the sibling functions' drafts/landed code for that accumulator shape before trying statement orders.
- (loop near1) Per-pass index local again: in PopUpMessageHandler::Loop, giving the second counting loop and the section
  loop their own 'for (int j = 0; ...)' / 'for (int i = 0; ...)' instead of a shared function-scope i fixed the
  'li r23,0; li r24,0' init (ours copied count into i) and the section/i init order; 'topMessage++, slot++' fixed the
  IV increment order; 'int messageTop = Get_top()' (not short) fixed the extsh order. Real 43 -> 27.
- (loop near1) brchk.py compiles .c drafts with SUFFIX=c and gets COMPILEFAIL; score.py compiles them as C++, which works
  (all three are clean). For drafts of a different LENGTH, scratch/loop3/near1/tools/brchk2.py aligns ours/orig with
  difflib before comparing targets; al.py prints the aligned listing. A switch whose case blocks (and string pool) are
  in a different order shows up there even when the logic is right (ShellInit: case 0/default first).
- (loop fresh3) A colour/struct copied as a WORD into a stack slot and then read back BYTE by byte (lwz/stw 8(r1), lbz 8..11(r1)): an
  inline getter that copies the word into a local through a cast ('T c; *(unsigned int*)&c = *(unsigned int*)&m; return c;' - the
  cast keeps the local in memory) followed by a byte-wise assignment. EndDraw 19 -> 11, length exact. The byte store order follows
  the operator= statement order; the load-into-temp order is irrelevant (CW reschedules the loads after the stores' order).
- (loop fresh3) Chained assignments in drafts ('a = b = Create()'; declined) usually have an identical plain form through an EXISTING
  local of the right type ('frame = Create(); b = frame; a = frame;'), while member-to-member forms ('b = Create(); a = b;') reload.
- (loop knots1) INLINE PARAMETER ORDER for integer colouring too: Update_Picture_Buffers' third-plane sum (Data() + w*h +
  (w/2)*(h/2), data/width r7/r6 swapped) became exact only with a helper taking (height, width, data); 5 other orders 48-96.
  Not landed (audit rule: invented register-steering helper). Same lever as vec3's Turn(a, v).
- (loop knots1) FPR callee-set swaps in a long expression block: try every value as a DECLARED LOCAL vs a direct member
  read (2^n grid). CCDBObject::Init's extents: dropping the sx/sy/sz/px locals (py/pz/q* kept) put sz and the 0.5 double
  into the orig's callee regs (real 41 -> 22); decl/statement orders had been inert for both forms.
- (loop land2) STACK-SLOT ORDER = INLINE EXPANSION ORDER. MWCC 1.3.2 lays out by-value struct locals/temps (ascending
  addresses) as: blocks of inline expansions, LATEST expansion lowest, each block in forward creation order (its locals,
  then its call-argument copies); the function's own locals and temps sit above all blocks in REVERSE creation order.
  Deeper inline levels expand later (lower): Update's sqrtf float temps ordered if-dist, else-dist, speed1, speed2 only
  once the speed block was itself an inline (GetSpeed -> sqrtf one level deeper). Reading the orig offsets as groups tells
  you which statements were inline helpers. SoundSchedule::Update 50 -> 0 with five single-use inlines
  (NodeAzimuth/NodeDistance taking the record so record->node reloads, PositionAzimuth/PositionDistance taking const
  CVector3&, ObjectSpeed). scratch/loop3/land2/tools/slots.py prints the r1-offset sequence ours vs orig.
- (loop mid3) Volatile-register rotation where one pointer local takes the LOWEST register in ours but the highest in the orig
  (SNDVOICEI_free: v r4 vs r8): the orig did not keep that local live across the function; pass the address expression
  again at each use ('releasevoice(&VOICES[voice])' instead of 'releasevoice(v)'), keeping the local only for its first
  reads (44 -> 1). A last 'cmpw rA,rB' operand swap that '==' operand order does not move: read through a per-entry pointer
  'T* q = &p[i]' (-> 0, landed 7dcdeb04).
- (loop mid3) FPR colouring set by DECLARATION order of locals first assigned much later: PopUp Loop's sy0..sy3 (used only in
  a later block) declared right after scaleX, before left/right/top/bottom, matched the orig's f29/f30 vs f26/f25 (27 -> 13).
  A value reused across two expressions written as a named local ('float offsetX') can be the knot: writing the
  product out per use ('left = 10*sx + (float)screenX * vp.m_x; right = ...') gave the orig temp numbering (6 -> 0).
- (loop mid3) An EMPTY switch case that the orig keeps as its own node ('cmpwi 8; beq; bge; b', all to the join): only an
  identity statement keeps it in CW 1.3.2. break, ';', '{}', '(void)0', 'if (0) {}', an empty inline call, a dead store to
  a register local, extra/merged case labels and default placement all prune the node (-1 instruction). Policy question.

## natural1 (2026-11-12): extra statement for inline depth
When a self-recursive function's inline depth needs one more statement, naming the values passed
to a call (`int count = ...; ... printf(fmt, count, ...)`) adds it without changing the code.
Replaced debug_prof's `(void)0;` (cf37912d).
- (loop slots1) Applying land2's slot reading: (a) when ours and the orig have IDENTICAL r1 offsets but a different frame
  size, the extra bytes are a struct view's size, not a temp: CScene::Draw's CDrawContext view was 292 -> 304 aligned
  (the matched Moh2.cpp view is 288); fixing it gave 416 = orig. (b) A by-value argument's ctor temp and its param copy
  form ONE block per argument; MWCC evaluates by-value args right to left but lays the blocks out left to right
  (first argument highest), so "built first AND highest" cannot come from one call's arguments (PlayerScreenFlash).
  (c) A dead struct temp survives only if the source reads it: a discarded result, an unread const& binding or an
  unused local all lose the stores. (d) Inline float& params do NOT keep vector locals in memory (promoted, as do
  float[3] arrays, float* params, a CVector3& + axis-index inline); only a returned reference (operator[]) or a
  pointer alias pins them (UpdateBoundingBox).
- (loop knots2) LOAD A REPEATED READ INTO A LOCAL, THEN 'q += ext' IN PLACE: the second read of a vector that the original
  reloads after stores that may alias it ('lfs f0..f2' then 'fadds f0,f0,..' into the SAME registers) is
  'qx = pos.x; qy = ..; qz = ..; qx += x; qy += y; qz += z; m_max.x = qx; ..'. Both 'm_max.x = qx + x' and the direct read
  'pos.x + x' give a fresh result register. For the FIRST read, ALL components as locals read before the long expression
  (knots1 had px direct). Declaring every local x, y, z, px, py, pz, qx, qy, qz was enough; decl orders were inert.
  CCDBObject::Init 22 -> 0, landed f748ce87 (cdi/g1.py, g3.py).
- (loop knots2) Hoisting every local of a function to the top and hill-climbing the declaration order (swaps + moves,
  pu/hc.py) moved an outer loop counter from a volatile register to the original's callee r29 (PROFILE_UPDATE 88 -> 82).
- (loop mid4) Undo earlier "steering" orders before hunting new ones: GetClosestParameter's draft had y,x,z orders in its inlines and
  dot products (an old local optimum); putting EVERY component sequence back in plain x,y,z order took it 94 -> 33 (each single
  change alone looked worse or flat: coordinate-descent from the steered draft got stuck). Plus a per-loop 'float u' -> 32.
- (loop mid4) Value-wrapper replacement: a 'MulV(a,b){return a*b;}' product kept apart from its load register is reproduced by NAMED
  float locals ('float tvx = fT * e1.x; ... tv.x = tvx;'): CCDBTriangle::TestCollision(CVolCapsule) MulV 10 / plain 28 / locals 7.
- (loop mid4) Accessor-wrapper replacement: AnimStStartState's invented '_St(o,k)' + identity 'o = owner' copy (10) = a natural
  'AnimStateSlotView* slots = owner->slots;' in the loop body plus '(unsigned short)id == slots[k].state' operand order (10).
- (loop mid4) A chained 'iresult = (int)(filter = 0);' in BIFunc_GetObjInfo was not needed: 'filter = 0; iresult = 0;' scores the
  same 16 ('iresult = 0; filter = 0;' 127). Check old chained/identity forms against the plain order both ways before keeping them.

## Shared zero 'li rA,0; mr rB,rA': the rule (zero1, 2026-11-12; probes scratch/loop3/zero1/p/*.cpp, pc.py + summ.py)
MWCC 1.3.2 emits one 'li' and copies it ONLY when the zero goes into a compiler-made variable: a
re-assigned user variable's later live range (a split web) or an inline's local/by-value param.
A user variable's first (or only) live range always gets its own 'li' (or is const-propagated);
'x = y', 'x = y = 0', casts, block scopes, decl order cannot change that.
- Shares: two variables that ALREADY HELD a live earlier value, re-zeroed together
  ('most = 0; weapon = 0;' after the weapon search, probes R1/R8/R9/R10/T2/T5); inline locals or
  by-value params initialised 0 (C, E, G, K, M, H); reused var + inline local (T4). int, unsigned,
  bool, pointer all the same (P1, B1).
- Does NOT share: fresh variables (A, B, I, J, R4, R11); only ONE of the two reused (R2, R3, T1);
  the earlier value was copy-propagated away so the variable never existed (R6, R7: 'h = w[0];
  msg(h)' then 'h = 0'); a call between the two zero assignments (T3); assigning a by-REF inline
  param 0 (F).
- Which gets the li: the first ASSIGNED (R1 vs R9) / first declared inline local (C vs G); the
  others 'mr' from it. Three re-zeroed vars: one li + two mr (T5).
- Recipe: if the two variables can naturally be the earlier search's variables, REUSE them
  ('most = 0; weapon = 0; for (j...) ...; ev = weapon * 5 ...'). CalculateLevelStats 1 -> 0, and
  GetCumulativeStats keeps 0 with the same form, so its inferred MostHit inline was dropped. Reuse
  also fixes the callee register (r29 = the dead walking pointer of the first loop), where the
  inline form landed in r30.
- Agrees with the parks lane's AnimFileRelocate toy (s19/t/t16): a loop variable REUSED from an earlier loop gives the
  'li i; mr off,i' init. Scan of all research/drafts (zero1/scan.py, pairs within 4 and 11 lines): CalculateLevelStats
  was the only draft whose original has a shared zero that ours lacks; the other hits were IV inits or colouring.
- Candidates for natural-form cleanup (landed with the inline-own-locals trick; try plain variable reuse instead):
  CPlayerObject::OnCollision PickUp, PATHI_crossfadetracks, SNDBANKI_playpatch, TriggerActivateMessages SendMessages
  defaults, list ctors' init_free, CSoldierObject::OnCollision FindClasses (-1 constant: probe untested).
- (zero2) Shared zero with a loop index: a function-level `i` used by an EARLIER loop and re-set by `for (i = 0; ...)`
  shares its li with a literal 0 stored inside that loop ('li r17,0; mr r16,r17', SNDBANKI_playpatch); no extra
  variable needed. A loop-carried variable zeroed at the top of each iteration does NOT count as reused (one web).
- (loop knots3) Negative: knots2's vector-component locals and 'q = v.x; q += e' in-place forms did not move any of the parked FP
  knots (GetExtents, Orthonormalize, ConstrainVelocity, aim blend, box-sphere/capsule, CCDBTriangle-capsule). Where a draft needs Mul/MulV,
  the product is an inline RETURN TEMP; plain locals, 'float r = a; r *= b;' statements (worse: 39-60), Set(x,y,z) param temps and getter
  inlines all colour differently. For an inline whose out-of-line copy is LANDED (BuildRot), score both together (knots3/rot/g.py):
  any body change must keep the landed function exact. Tool: knots3/thc.py = generic line-order hill climb with def/use checks.
- (zero3) Negative: the shared-zero rule and loop-index reuse do not move ResetShell's 0/1 r3/r4 swap (enum/default-arg/
  local-copy type, flag setter inlines, function-level i: all 18), quick_sort_'s tail next (6), CPathNodeList's pool-base
  rank (99) or CheckForObstructions' sign rank (ternary 25-26). Three of these share one pattern: a later value ranks ABOVE
  'this' (r31) in the original; ours always gives 'this' r31 (also SetFinishedLevelData's i). scan: zero3/scan2.py.
- (loop scene1) CONSTRUCTOR COLOURING FROM EXCEPTION RECORDS: with -Cpp_exceptions on, a ctor whose members have (even inline)
  destructors gets one cleanup record per constructed member, and that alone changes register allocation: CScene::CScene 92 -> 0
  (0/1/2 into callee regs, sentinels into volatile) after only adding the containers' inline dtors; every spelling was inert
  before. Read the original's extab record (extabindex third word -> objdump -s -j extab): its action records name the
  destructors (and so the member/element types) the source had. Also check the out-of-line call's arguments when a loop
  calls through a pointer-to-member: CScene::Draw's draft dropped the node argument (check_draft cannot see it).

## Callee-register RANK: who outranks 'this' (rank1, 2026-11-12; probes scratch/loop3/rank1/p/*.cpp, pc.py)
Base rule in SMALL functions (low pressure), measured, use counts and loop depth do NOT matter:
- Declared locals outrank ALL parameters ('this' included): C1/C5/D2/E6-E8 (a local used 2x beats a param used 20x, K1-K20).
- Locals among themselves: DECLARATION order (first declared = r31), not first assignment (E2). Outer loop index
  above inner (C3), whatever the use counts (C2b, C3b).
- Parameters among themselves: REVERSE order, last parameter highest (D1, E5); so 'this' is normally the LOWEST.
- Locals of an expanded inline outrank the caller's locals (N2: inline z r31 > caller a r30 > this r29).
- A loop-invariant constant base hoisted by CW (lis/addi of a .rodata table read in a loop, or a NextPrime-style table
  inline + const-table initialiser in a later loop) outranks everything (H1-H8, Q1/Q2: base r31 > i > k > this).
  => the originals' "later value above 'this'" is the NORMAL order; what needs explaining is OUR 'this' at r31.
Pressure (Chaitin degree) promotes: a value interfering with many simultaneously-live values jumps to the top.
- K probes: 'S* p' live across N volatile loads + xor chain, local j after: N<=8 -> j r31 > p; N>=9 -> p r31.
- L probes (sequential pairs): ONLY M=8 flips (p above v), M=7 and M>=9 do not: exactly the signature of a degree
  threshold (one node >= K while its neighbour is still < K; when both are >= K the index order returns). M probes:
  with both values over the threshold the order becomes erratic (M16/M20).
- So in big functions 'this' (live everywhere) is promoted by degree; a later value beats it only if it is ALSO over
  the threshold and pushed later. Which side of the threshold a value sits on is set by its live range, not its type.
Lever found (CheckForObstructions): 'int sign = 1; if (IsAbove(*this, node)) sign = -1;' (def BEFORE the IsAbove inline)
  gives the orig ranks exactly (sign r31, this r30, node r28 - the longer range crosses the threshold), but CW then keeps
  the 'li 1' before the inline: 25 -> 36. The same li placed after the inline (bool local, block scope, decl at top,
  'sign = -sign', offset/height order) is back to sign r28. The orig must lengthen sign's range in the IR some other way.
- Lead from the coordinator checked: CPathNodeList's extab record is '600a0000 00000000' in BOTH the original and ours
  (no cleanup actions), so member destructors do not explain its rank (scratch/loop3/rank1/extab.py <addr>,
  ourext.py <json> <draft>).
- DMClusterSkelTraverseHeirarchy's orig order (count, bone > params reversed > base, depth, a, b, c) and SetFinishedLevelData
  (mission, stage > i > this) do not follow the index rule either; both look like degree promotion of the long-lived
  values. Not solved: no landing from this study.

## extab records as type evidence (eh1, 2026-11-12)
- research/tools/extab_records.py (`orig ADDR`, `cmp ADDR TMPL SRC FUNC`) decodes the original's and our
  extab record: DLOCAL/DMEMBER/DBASE/DMEMBERARR name the destructors, DELPTR names the operator delete.
  A local the original destroys (DLOCAL) needs a destructor in the draft even if it is empty and inline:
  CW then emits the weak out-of-line copy for the table (Play: DISP_OLD_FRAMES, real name from the dtor
  symbol). DELPTR with __dl__FPv means the new'd class has NO class operator delete (ShellInit's CSprite).
- Header-only records (no ranges) mean no cleanup at all: nothing to learn for CreateRootHierObject,
  CreateDestructable, CreateDumpObject, AnimStStartState, CPathNodeList ctor.
- (loop undo1) Value-wrapper replacement for selects: a 'bge; b; fmr a,b' min/max that keeps the result in the term's
  own register needs copy-defined terms; FIELDS of a plain local CVector3 do that ('CVector3 s; s.x = start.x - radius;
  ... min.x = s.x < e.x ? s.x : e.x'). CVolCapsule::GetExtents SubV/AddV 0 / ctor form 32 / named floats 66 (bac20c93).
  Inline ctor arguments are evaluated right to left (z before y), which swaps the y/z colours.
- (loop undo1) A pointer select between two stack locals ('float* end = a < b ? &a : &b', loop reads *end) is an inlined
  reference-returning min: 'float end = dwi::min(a, b)' (body 'a < b ? a : b') fixed GetClosestParameter 32 -> 0.
- (loop minmax1) Census: of the 112 open drafts, only CCDBVolBox::TestCollision(CTriangle) has the stack-slot
  reference min/max select (already written as ref Min/Max, select exact, colours upstream), and only UpdateBoundingBox
  has 'bge; b; fmr' selects. scratch/loop3/minmax1/scan.py re-runs the scan (hits.tsv).
- (loop real2) Real CVector3 members in drafts: box-plane went 52 -> 0 once the invented MulV/operators were replaced with
  a.Scale(axis, half), corner copy + corner.Sub(corner, v)/Add(v), AND the normal's Negate moved before the corner subtractions
  (statement order mattered more than the helper: Scale-only 120, Negate after 149). Per-component hill-climb orders from vec7 were
  approximating member-call ordering. Elsewhere the members were neutral (renames at equal score); value forms like n.Sub then
  n.Scale(n, inv) in place of float triples lose (kept in memory).
- (loop real3) DECLARATION order of plain float locals is a lever for FPR colouring even when statement order is fixed:
  ConstrainVelocity 14 -> 6 only by reordering the 'float x;' declarations of the second-solve block (det, e, bd, n2&, py, px, n1&,
  dc, d2, inv, pz). Tool: real3/ohc.py KEY base.cpp spec.py seed N, spec = "BLOCKFILE='block.txt'; exec(open('gspec.py').read())"
  (splits 'T x = e;' into decl + assignment, deps from def/use of block locals; ~0.1-1 s per compile; reblock.py restarts from a best).
  CCDBTriangle-capsule 7 -> 5: rv.Scale(dir, fR) hoisted before the edge products (member-call order) + decl order.
  Inert under the same climb: OnCollision (3), box-sphere top block (13), box-capsule n block (102), death_cb loop inits.
  Box-sphere: 'CVector3 local(m_axes[0].Dot(diff), m_axes[1].Dot(diff), m_axes[2].Dot(diff))' (3-float ctor, args right to left)
  gives the original's compute-z-first/store-x-first order, but scores 19 (vs 13): y and abs.x colours swap.
- (fpr2) Two FPRs swapped while the SCHEDULE already matches (orig 'fmuls f6,A; fmuls f8,B', ours 'fmuls f6,B; fmuls f8,A'): CW colours in value-creation order and schedules separately, so swap which of the two statements comes FIRST in the source (move the one the orig gives the lower reg earlier); the scheduler keeps the emitted order (ConstrainVelocity 2 -> 0).
- (pum3) A named local both stored to a member and reused later ('extra = entry[0]; rec.m_extra = extra; ... + extra') colouring against a hoisted global base in swapped volatile regs: write the chained assignment 'extra = rec.m_extra = entry[0];' (the stored value is the temp, extra its copy; one load). PopUpMessageHandler ctor 16 -> 0 where ~1,500 order/type variants were inert.

## sinit1 (2026-11-12): an __sinit that stores a callee FPR it never loads
That is a temporary's uninitialised float members kept in a register. Flatten the temporary's
sub-object members (plain colour bytes instead of a CColor member) rather than hunting for a
0.0f. __sinit_pumhandler_cpp (fac4d7fa).

## extab records as type evidence (eh1, 2026-11-12)
- research/tools/extab_records.py (`orig ADDR`, `cmp ADDR TMPL SRC FUNC`) decodes the original's and our
  extab record: DLOCAL/DMEMBER/DBASE/DMEMBERARR name the destructors, DELPTR names the operator delete.
  A local the original destroys (DLOCAL) needs a destructor in the draft even if it is empty and inline:
  CW then emits the weak out-of-line copy for the table (Play: DISP_OLD_FRAMES, real name from the dtor
  symbol). DELPTR with __dl__FPv means the new'd class has NO class operator delete (ShellInit's CSprite).
- Header-only records (no ranges) mean no cleanup at all: nothing to learn for CreateRootHierObject,
  CreateDestructable, CreateDumpObject, AnimStStartState, CPathNodeList ctor.
- (loop undo1) Value-wrapper replacement for selects: a 'bge; b; fmr a,b' min/max that keeps the result in the term's
  own register needs copy-defined terms; FIELDS of a plain local CVector3 do that ('CVector3 s; s.x = start.x - radius;
  ... min.x = s.x < e.x ? s.x : e.x'). CVolCapsule::GetExtents SubV/AddV 0 / ctor form 32 / named floats 66 (bac20c93).
  Inline ctor arguments are evaluated right to left (z before y), which swaps the y/z colours.
- (loop undo1) A pointer select between two stack locals ('float* end = a < b ? &a : &b', loop reads *end) is an inlined
  reference-returning min: 'float end = dwi::min(a, b)' (body 'a < b ? a : b') fixed GetClosestParameter 32 -> 0.
- (loop minmax1) Census: of the 112 open drafts, only CCDBVolBox::TestCollision(CTriangle) has the stack-slot
  reference min/max select (already written as ref Min/Max, select exact, colours upstream), and only UpdateBoundingBox
  has 'bge; b; fmr' selects. scratch/loop3/minmax1/scan.py re-runs the scan (hits.tsv).
- (loop real2) Real CVector3 members in drafts: box-plane went 52 -> 0 once the invented MulV/operators were replaced with
  a.Scale(axis, half), corner copy + corner.Sub(corner, v)/Add(v), AND the normal's Negate moved before the corner subtractions
  (statement order mattered more than the helper: Scale-only 120, Negate after 149). Per-component hill-climb orders from vec7 were
  approximating member-call ordering. Elsewhere the members were neutral (renames at equal score); value forms like n.Sub then
  n.Scale(n, inv) in place of float triples lose (kept in memory).
- (loop real3) DECLARATION order of plain float locals is a lever for FPR colouring even when statement order is fixed:
  ConstrainVelocity 14 -> 6 only by reordering the 'float x;' declarations of the second-solve block (det, e, bd, n2&, py, px, n1&,
  dc, d2, inv, pz). Tool: real3/ohc.py KEY base.cpp spec.py seed N, spec = "BLOCKFILE='block.txt'; exec(open('gspec.py').read())"
  (splits 'T x = e;' into decl + assignment, deps from def/use of block locals; ~0.1-1 s per compile; reblock.py restarts from a best).
  CCDBTriangle-capsule 7 -> 5: rv.Scale(dir, fR) hoisted before the edge products (member-call order) + decl order.
  Inert under the same climb: OnCollision (3), box-sphere top block (13), box-capsule n block (102), death_cb loop inits.
  Box-sphere: 'CVector3 local(m_axes[0].Dot(diff), m_axes[1].Dot(diff), m_axes[2].Dot(diff))' (3-float ctor, args right to left)
  gives the original's compute-z-first/store-x-first order, but scores 19 (vs 13): y and abs.x colours swap.
- (fpr2) Two FPRs swapped while the SCHEDULE already matches (orig 'fmuls f6,A; fmuls f8,B', ours 'fmuls f6,B; fmuls f8,A'): CW colours in value-creation order and schedules separately, so swap which of the two statements comes FIRST in the source (move the one the orig gives the lower reg earlier); the scheduler keeps the emitted order (ConstrainVelocity 2 -> 0).
- (pum3) A named byte local stored to a member AND reused later ('extra = entry[0]; rec.m_extra = extra; ... + extra') came out in the wrong volatile reg against a hoisted global base (r7/r6 swapped), inert to ~1,500 order/type variants. The chained assignment 'extra = rec.m_extra = entry[0];' makes the stored value the temp and extra a copy of it: one load, original colouring (PopUpMessageHandler ctor 16 -> 0; the out-of-line Load stays exact with it).
- (sinit2) CW 1.3.2 can scalarise a temporary PARTLY: members it can split (init-list scalars) leave no stack stores and
  reach copies as constants, while a region it cannot split stays at its offset in the temp's slot. Seen so far only when
  that region is a never-written array read by an implicit copy (pumhandler's text) or is WRITTEN by a block copy
  (value-init 'state(BuddyState())' of a POD member, or 'state(g_x)': three lwz/stw words). Any address of the temp or a
  sub-object (self = this, &member, subscripts, member ctor this in an __sinit) makes the whole temp memory; plain typed
  accesses scalarise everything. buddy.cpp's BuddyInfo() temp (sinit, list range ctor, Create) is such a partial temp
  with TYPED region stores; no natural source found yet (scratch/loop3/sinit2/notes.md).
- (g1b, 2026-11-13) A dead stack copy that the scheduler places BEFORE the assigned object's second half: with a by-value operator= the return temp is created after the whole assignment; an inline that copies through a local first ('T t = s; d = t;' with a reference operator=) creates it before. CAnimMorph::Draw 3 -> 0.

## An inline's locals: reversed declaration order when expanded (dtor1, 2026-11-13; probes scratch/loop3/dtor1/p, pr.py, v1.sh)
MWCC 1.3.2 numbers the locals of an EXPANDED inline in the reverse of their declaration order,
relative to the same function compiled out of line. So a weak out-of-line copy and its inlined
copies in the same file colour two locals opposite ways whenever the locals sit directly in that
function's body. Probe (dwi::list<Info>, `~list` loop with `node`/`next`, `reserve` destroying a
local list; target out-of-line node r6/next r5, inlined node r5/next r4):
- locals in `~list` itself: next-first gives the out-of-line copy right and the inlined one
  swapped (a, h, i, l), node-first the opposite (b, c, d, g, k). Scoped/for/while/helper-for-the-
  three-stores forms only move which side is right; '-inline deferred,auto' is the same (da, db).
- locals in an inline that `~list` calls (`~list() { clear(); ... }`): BOTH copies are expanded
  inlines, so one declaration order gives both (f: node first; f2: next scoped inside the loop;
  f3: with `node->~list_node<T>()`). next-first in clear() is wrong on both (e). Depth does not
  re-flip: clear() calling clear2() behaves like clear() (m BOTH, m2 neither).
- Not the cause (ruled out): another TU's weak copy (each file record holds its own ~list<X>; the
  orders reproduce in a one-file probe), different flags (every list/reserve fragment pair has the
  same flags: alarm/pumhandler/world_object/world_volume '-inline auto', bsutil deferred,auto), or a
  degree/pressure effect (rank1's threshold): same 4 values, same live ranges in both copies.
- Recipe: when an out-of-line copy and an inlined copy of one function need opposite local orders,
  move the locals into an inline the function calls (a real member such as clear(), or a single-use
  inline helper, accepted form) and write them in the INLINED copy's order. bsutil's
  DoWeOwnThisMemory: out-of-line (owned, base, offset) and the copy in
  BSUtilFreeObjectInstanceMemory both match with an inline OwnsElement declaring offset, base, owned
  (bsu/var2.py: only the f-b-o declaration order, any assignment order). alarm: all 11 functions
  with `~list() { clear(); ... }`. include/game/DwiContainers.h NOT changed: its ~list through
  clear() keeps all 105 header units' code exact (vh.py), but renumbers the compiler's @N data
  labels, so scene_dtor's stripped_objects '@177' no longer names one object and reconstruct
  fails; changing it needs the manifests that name @N labels updated in the same commit.
- world_volume after the fix: all 27 functions exact but the weak copies' ORDER still differs
  (original ~CWorldVolume, ~list, ctor, reserve; ours ~CWorldVolume, ctor, reserve, ~list with
  reserve in-class, wv/c2.cpp). Out-of-class/inline/weak ~list and deferred+reversed sources
  (wv/rc*.cpp, TestCollisionWithSubVolumes then gets inlined) did not place ~list before the ctor.
- pumhandler: the clear() form makes the list fragments' pair exact in one compile (pumhandler_list's
  ~list<QueuedSubtitle> and pumhandler_list_reserve, pw/pumhandler_lr.cpp); the whole-file draft is
  not written yet. world_object (callee registers: the loop calls ~CCompartment): with clear() the
  out-of-line ~list<CCompartment> is exact and reserve's node/next keep the right relative order, but
  both sit one register lower (r26/r25 for r27/r26) whatever the clear() spelling (pw/wov.py, 7
  forms): the second inline level seems to add one ranked value. Open.

## Deferred-mode emission order, colour copies, world_object's reserve rank (merge9, 2026-11-13; scratch/loop3/merge9)
- With '-inline deferred(,auto)' functions come out in reverse source order, and a weak copy of an
  IN-CLASS inline member comes out right after the first function (in output order) that needs it
  out of line, followed by what it needs itself (pumhandler: DisplaySubtitle, reserve, ~list, the
  range constructor, UpdateSubtitles). Out-of-class template members and member templates go to
  the END of the file; an explicit 'template class X<T>;' places the ordinary members where it
  stands but not the member templates. So define every list member in the class.
- Deferred evidence without RTTI: function statics and the .bss destructor-chain records (12
  bytes) numbered below every extab label (pumhandler: @648, @716.., @880, @936 < @1128).
- An __sinit storing uninitialised floats from f31: set the temporary's colour in its
  constructor BODY ('m_color.r = 0; ...'), not with a CColor(0,0,0,128) initialiser (which takes
  the member's address and keeps the temporary on the stack).
- A CColor copied as one word to the stack and read back by bytes = passed BY VALUE (an inline
  taking CColor); 'CColor c = node->m_data.m_color;' with game/CColor.h copies member-wise and
  propagates the bytes (UpdateSubtitles 96 -> 0 with an inline ShowSubtitle(..., CColor)).
- world_object's reserve<CCompartment> (clear() loop one callee register low, r26/r25 for
  r27/r26): split the range constructor into the inline helpers init_free() and link_range(), as
  pumhandler/alarm, with 'const T& value = *it;' before the placement new: reserve and the
  out-of-line ~list both exact (scratch/loop3/merge9/wo/h3.cpp).
- world_volume (open): the original is deferred with RTTI (RTTI labels @364/@365 below extab
  @386) and ~CWorldVolume inlines ~list, yet the weak ~list comes out right after it; explicit
  instantiation after ~CWorldVolume, ': m_subVolumes()', ~list out of class (plain/weak/inline)
  are all inert.

## merge11 (world_object, world_volume)
- One dwi::list template for a POD element and a class element (world_object reserve<SRequest> and
  reserve<CCompartment>): range ctor 'link_range(end(), first, last)' with 'head = end.m_node;
  stop = last.m_node; tail = head->m_prev;' and end() computed from this+16. Taking head from
  &m_head lets it be hoisted (SRequest: r0 vs r7). Taking tail from m_head.m_prev loads it
  frame-relative (CCompartment 128(r1) vs 16(r28)).
- If the element class really derives from a class with a virtual destructor, 'x.~T()' becomes a
  virtual call. The original's direct call comes from destroying the node ('node->~list_node<T>()').
- A struct with an array member is not scalarised: a local built and then copied into a node keeps
  its stack copy (world_object Acquire: SRequest's 'unsigned char m_reuse[4]', 79 diffs as bool).
- Deferred + written last to first: global objects with destructors defined at the END of the
  source put their dtor-chain records after the other .bss temporaries.
- world_volume: a weak in-class copy comes out right after a dead-stripped function. Its order needs
  a function in the source between ~CWorldVolume and the constructor that needs ~list out of line.
  Large extab label gaps between neighbouring functions are the sign.

## late2 (2026-11-13; scratch/loop3/late2b)
- A colour (four bytes) stored as ONE word to the stack and read back by bytes in a non-member order (EndDraw: g, b, a, r):
  return it from an inline that copies the word into an INTEGER local and returns that local viewed as the struct
  ('unsigned int w = *(unsigned int*)&color; return *(CColor*)&w;'), with the shared CColor.h's implicit copy. Copying the
  word into a CColor local ('CColor c; *(unsigned int*)&c = ...'), 'return color', 'CColor c = color' and byte-order copy
  ctors give other load orders (ed/v1-v7).
