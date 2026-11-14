from sxv import *
S=open('sx/c_none.cpp').read()
a=S.index('static void SweepExtents('); b=S.index('\n}\n',a)+3
fn=S[a:b]
# member begin/end + iterator typedef
S2=S.replace('    list_node<T> m_head;\n    bool m_owned;\n};','    list_node<T> m_head;\n    bool m_owned;\n};',1)
assert 'class list {\npublic:\n' in S2
S2=S2.replace('class list {\npublic:\n','class list {\npublic:\n    typedef list_iter<T> iterator;\n    iterator begin() { return iterator(m_head.m_next); }\n    iterator end() { return iterator(&m_head); }\n',1)
# list_iter must be declared before list: move ITER block before list
i0=S2.index('namespace dwi {\ntemplate <class T>\nclass list_iter'); i1=S2.index('}\n',S2.index('inline list_iter<T> lend'))+2
it=S2[i0:i1]; S2=S2[:i0]+S2[i1:]
it=it.replace('template <class T>\ninline list_iter<T> lbegin(list<T>& l) { return list_iter<T>(l.m_head.m_next); }\ntemplate <class T>\ninline list_iter<T> lend(list<T>& l) { return list_iter<T>(&l.m_head); }\n','')
j=S2.index('template <class T>\nclass list {')
S2=S2[:j]+it[len('namespace dwi {\n'):-2]+S2[j:]
f2=fn.replace('dwi::list_iter<ISceneNode::SExtent> node;','dwi::list<ISceneNode::SExtent>::iterator node;').replace('dwi::lbegin(extents)','extents.begin()').replace('dwi::lend(extents)','extents.end()')
S3=S2[:S2.index('static void SweepExtents(')]+f2+S2[S2.index('static void SweepExtents(')+len(fn):]
print('member',score(S3,'m_member'),flush=True)
# Pairs inline call
i=f2.index('            for (it = active.begin();'); j=f2.index('            active.push_back(sceneNode);')
f3=f2[:i]+'            Pairs(active, sceneNode, pairs, exclusions, it);\n'+f2[j:]
S4=S3[:S3.index('static void SweepExtents(')]+f3+S3[S3.index('static void SweepExtents(')+len(f2):]
print('pairs',score(S4,'m_pairs'),flush=True)
