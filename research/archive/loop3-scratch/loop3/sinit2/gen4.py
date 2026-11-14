import itertools, os, sys
D = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(D, 'v5')
os.makedirs(OUT, exist_ok=True)
pre = open(os.path.join(D, 'pre.h')).read()
bl = open(os.path.join(D, 'base_l.h')).read()
bs = open(os.path.join(D, 'base_s.h')).read()
SD = {'none': '',
      'body': 'BuddyState() { moving = 0; active = false; path = 0; }',
      'self': 'BuddyState() { BuddyState* self = this; self->moving = 0; self->active = false; self->path = 0; }',
      'init': 'BuddyState() : path(0), moving(0), active(false) {}',
      'empty': 'BuddyState() {}'}
SC = {'impl': '',
      'body': 'BuddyState(const BuddyState& o) { moving = o.moving; active = o.active; path = o.path; }',
      'self': 'BuddyState(const BuddyState& o) { BuddyState* self = this; self->moving = o.moving; self->active = o.active; self->path = o.path; }',
      'init': 'BuddyState(const BuddyState& o) : path(o.path), moving(o.moving), active(o.active) {}'}
SA = {'impl': '',
      'body': 'BuddyState& operator=(const BuddyState& o) { moving = o.moving; active = o.active; path = o.path; return *this; }'}
BD = {'none': '',
      'sp': 'BuddyInfo() : soldier(0), progress(0) {}',
      'spn': 'BuddyInfo() : soldier(0), progress(0), state(BuddyState()) {}',
      'spa': 'BuddyInfo() : soldier(0), progress(0) { state = BuddyState(); }',
      'spf': 'BuddyInfo() : soldier(0), progress(0) { state.moving = 0; state.active = false; state.path = 0; }',
      'bsp': 'BuddyInfo() { soldier = 0; progress = 0; }',
      'bspa': 'BuddyInfo() { soldier = 0; progress = 0; state = BuddyState(); }'}
BC = {'impl': '',
      'u1': 'BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress), state(o.state) {}',
      'u2': 'BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress) { state = o.state; }',
      'u3': 'BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress) { state.moving = o.state.moving; state.active = o.state.active; state.path = o.state.path; }'}
kind = sys.argv[1] if len(sys.argv) > 1 else 's'
for (a, sd), (b, sc), (e, sa), (c, bd), (d, bc) in itertools.product(SD.items(), SC.items(), SA.items(), BD.items(), BC.items()):
    if c in ('spf',) and a in ('self',):
        pass
    if a == 'none' and c in ('spn', 'spa', 'bspa') and sc != 'impl':
        continue
    body = '''struct BuddyState {
    %s
    %s
    %s
    int path; int moving; bool active;
};
struct BuddyInfo {
    %s
    %s
    void* soldier; int progress; BuddyState state;
};
''' % (sd, sc, sa, bd, bc)
    name = '%s_%s_%s_%s_%s' % (a, b, e, c, d)
    if kind == 's':
        open(os.path.join(OUT, name + '_s.cpp'), 'w').write(body + bs)
    else:
        open(os.path.join(OUT, name + '_l.cpp'), 'w').write(pre + body + bl)
