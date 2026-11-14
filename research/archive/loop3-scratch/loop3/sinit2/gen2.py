import itertools,os
from gen import write
SD={'self':'BuddyState() { BuddyState* self = this; self->moving = 0; self->active = false; self->path = 0; }',
    'body':'BuddyState() { moving = 0; active = false; path = 0; }',
    'bodyp':'BuddyState() { path = 0; moving = 0; active = false; }',
    'init':'BuddyState() : path(0), moving(0), active(false) {}',
    'none':''}
SC={'body':'BuddyState(const BuddyState& o) { moving = o.moving; active = o.active; path = o.path; }',
    'self':'BuddyState(const BuddyState& o) { BuddyState* self = this; self->moving = o.moving; self->active = o.active; self->path = o.path; }',
    'init':'BuddyState(const BuddyState& o) : path(o.path), moving(o.moving), active(o.active) {}',
    'impl':''}
BD={'nest':'BuddyInfo() : soldier(0), progress(0), state(BuddyState()) {}',
    'nestb':'BuddyInfo() : soldier(0), progress(0) { state = BuddyState(); }',
    'dflt':'BuddyInfo() : soldier(0), progress(0) {}',
    'bodyall':'BuddyInfo() { soldier = 0; progress = 0; state = BuddyState(); }'}
BC={'impl':'','user':'BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress), state(o.state) {}'}
for (a,sd),(b,sc),(c,bd),(d,bc) in itertools.product(SD.items(),SC.items(),BD.items(),BC.items()):
    if a=='none' and c=='dflt': continue
    body='''struct BuddyState {
    %s
    %s
    int path; int moving; bool active;
};
struct BuddyInfo {
    %s
    %s
    void* soldier; int progress; BuddyState state;
};'''%(sd,sc,bd,bc)
    write('g_%s_%s_%s_%s'%(a,b,c,d),body)
