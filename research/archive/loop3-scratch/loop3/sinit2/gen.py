import sys,os
# variants: name -> type defs
V={}
V['flat_self']='''
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) { BuddyInfo* self = this; self->moving = 0; self->active = false; self->path = 0; }
    BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress) { moving = o.moving; active = o.active; path = o.path; }
    void* soldier; int progress; int path; int moving; bool active;
};'''
V['base_head']='''
struct BuddyHead { BuddyHead() : soldier(0), progress(0) {} void* soldier; int progress; };
struct BuddyInfo : BuddyHead {
    BuddyInfo() { BuddyInfo* self = this; self->moving = 0; self->active = false; self->path = 0; }
    int path; int moving; bool active;
};'''
V['base_head2']='''
struct BuddyHead { void* soldier; int progress; };
struct BuddyInfo : BuddyHead {
    BuddyInfo() { soldier = 0; progress = 0; BuddyInfo* self = this; self->moving = 0; self->active = false; self->path = 0; }
    int path; int moving; bool active;
};'''
V['base_head3']='''
struct BuddyHead { BuddyHead() : soldier(0), progress(0) {} void* soldier; int progress; };
struct BuddyInfo : BuddyHead {
    BuddyInfo() { moving = 0; active = false; path = 0; }
    int path; int moving; bool active;
};'''
V['state_nested']='''
struct BuddyState {
    BuddyState() { BuddyState* self = this; self->moving = 0; self->active = false; self->path = 0; }
    BuddyState(const BuddyState& o) { moving = o.moving; active = o.active; path = o.path; }
    int path; int moving; bool active;
};
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0), state(BuddyState()) {}
    void* soldier; int progress; BuddyState state;
};'''
V['head_member']='''
struct BuddyHead { BuddyHead() : soldier(0), progress(0) {} void* soldier; int progress; };
struct BuddyInfo {
    BuddyInfo() { BuddyInfo* self = this; self->moving = 0; self->active = false; self->path = 0; }
    BuddyHead head; int path; int moving; bool active;
};'''
V['flat_body']='''
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) { moving = 0; active = false; path = 0; }
    void* soldier; int progress; int path; int moving; bool active;
};'''
def write(name,body):
    open(os.environ.get('VD','v')+'/%s_s.cpp'%name,'w').write(body+'\n#include "base_s.h"\n')
    open(os.environ.get('VD','v')+'/%s_l.cpp'%name,'w').write('#include "pre.h"\n'+body+'\n#include "base_l.h"\n')
os.makedirs(os.environ.get('VD','v'),exist_ok=True)
if __name__=='__main__':
    for k,b in V.items(): write(k,b)
