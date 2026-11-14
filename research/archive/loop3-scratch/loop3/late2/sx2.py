from sxv import *
import itertools
ITER='''namespace dwi {
template <class T>
class list_iter {
public:
    list_iter() {}
    list_iter(list_node<T>* n) : node(n) {}
    list_iter(const list_iter& other) : node(other.node) {}
    T& operator*() const { return node->m_data; }
    T* operator->() const { return &node->m_data; }
    list_iter& operator++() { node = node->m_next; return *this; }
    bool operator!=(list_iter other) const { return node != other.node; }
    bool operator==(const list_iter& other) const { return node == other.node; }
    list_node<T>* node;
};
template <class T>
inline list_iter<T> lbegin(list<T>& l) { return list_iter<T>(l.m_head.m_next); }
template <class T>
inline list_iter<T> lend(list<T>& l) { return list_iter<T>(&l.m_head); }
}
'''
i=B.index('template <class T>\nclass fast_vec')
B2=B[:i]+'}\n'+ITER+'namespace dwi {\n'+B[i:]
import sxv
sig=FN[:FN.index('{\n')+2]
top=FN[FN.index('{\n')+2:FN.index('    active.UseBuffer')]
body=FN[FN.index('    active.UseBuffer'):]
lines=[l+'\n' for l in top.split('\n') if l.strip()]
NODE='    dwi::list_node<ISceneNode::SExtent>* node;\n'
assert NODE in lines
forms={
 'pre':('for (node = (*c_extents).m_head.m_next; node != &(*c_extents).m_head; node = node->m_next) {','for (node = dwi::lbegin(*c_extents); node != dwi::lend(*c_extents); ++node) {'),
 'prex':('for (node = (*c_extents).m_head.m_next; node != &(*c_extents).m_head; node = node->m_next) {','for (node = dwi::lbegin(extents); node != dwi::lend(extents); ++node) {'),
}
for fk,(o,nw) in forms.items():
  for nd in ['    dwi::list_iter<ISceneNode::SExtent> node;\n']:
    for p in range(len(lines)):
      L=[l for l in lines if l!=NODE]; L.insert(p,nd)
      bb=body.replace(o,nw).replace('node->m_data.node','(*node).node').replace('node->m_data.start','(*node).start')
      assert nw in bb
      src=B2[:B2.index('static void SweepExtents(')]+sig+''.join(L)+bb+B2[B2.index('static void SweepExtents(')+len(FN):]
      print(fk,p,score(src,f'it_{fk}_{p}'),flush=True)
