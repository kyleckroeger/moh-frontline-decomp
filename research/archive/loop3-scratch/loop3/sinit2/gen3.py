import itertools, os, subprocess, sys
D = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(D, sys.argv[1] if len(sys.argv) > 1 else 'v3')
os.makedirs(OUT, exist_ok=True)
pre = open(os.path.join(D, 'pre.h')).read()
bl = open(os.path.join(D, 'base_l.h')).read()
bs = open(os.path.join(D, 'base_s.h')).read()
NODE = 'list_node(list_node* prev, list_node* next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}'
HEAD = 'm_head(&m_head, &m_head, T())'
NODES = {
    'cref': NODE,
    'val': 'list_node(list_node* prev, list_node* next, T data) : m_prev(prev), m_next(next), m_data(data) {}',
    'pref': 'list_node(list_node* const& prev, list_node* const& next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}',
    'dflt': 'list_node(list_node* prev, list_node* next, const T& data = T()) : m_prev(prev), m_next(next), m_data(data) {}',
}
HEADS = {'t': HEAD, 'tt': 'm_head(&m_head, &m_head, T(T()))'}
SD = {'self': 'BuddyState() { BuddyState* self = this; self->moving = 0; self->active = false; self->path = 0; }',
      'body': 'BuddyState() { moving = 0; active = false; path = 0; }'}
SC = {'body': 'BuddyState(const BuddyState& o) { moving = o.moving; active = o.active; path = o.path; }',
      'self': 'BuddyState(const BuddyState& o) { BuddyState* self = this; self->moving = o.moving; self->active = o.active; self->path = o.path; }',
      'impl': ''}
BD = {'nest': 'BuddyInfo() : soldier(0), progress(0), state(BuddyState()) {}',
      'dref': 'BuddyInfo(const BuddyState& s = BuddyState()) : soldier(0), progress(0), state(s) {}',
      'dval': 'BuddyInfo(BuddyState s = BuddyState()) : soldier(0), progress(0), state(s) {}',
      'inpl': 'BuddyInfo() : soldier(0), progress(0) {}'}
BC = {'impl': '', 'user': 'BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress), state(o.state) {}'}
for (n, nd), (h, hd), (a, sd), (b, sc), (c, bd), (d, bc) in itertools.product(
        NODES.items(), HEADS.items(), SD.items(), SC.items(), BD.items(), BC.items()):
    if h == 'tt' and n != 'cref':
        continue
    body = '''struct BuddyState {
    %s
    %s
    int path; int moving; bool active;
};
struct BuddyInfo {
    %s
    %s
    void* soldier; int progress; BuddyState state;
};
''' % (sd, sc, bd, bc)
    name = '%s_%s_%s_%s_%s_%s' % (n, h, a, b, c, d)
    open(os.path.join(OUT, name + '_s.cpp'), 'w').write(body + bs.replace(NODE, nd).replace(HEAD, hd))
    open(os.path.join(OUT, name + '_l.cpp'), 'w').write(pre + body + bl.replace(NODE, nd).replace(HEAD, hd))
