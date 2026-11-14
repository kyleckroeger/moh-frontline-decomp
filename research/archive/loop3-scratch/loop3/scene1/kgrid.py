import itertools,sys,subprocess
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import score
base=open('k0.cpp').read()
knobs=[
 [("",""),("    list_node(const T& data) : m_prev(this), m_next(this), m_data(data) {}","    list_node(list_node* prev, list_node* next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}")],
 [("",""),("m_head(T()), m_owned(true) {}","m_head(T()) { m_owned = true; }")],
 [("",""),("m_capacity(0), m_owned(true), m_growth(growth) {}","m_capacity(0), m_owned(true) { m_growth = growth; }"),("m_capacity(0), m_owned(true), m_growth(growth) {}","m_capacity(0) { m_owned = true; m_growth = growth; }")],
 [("",""),("m_data(0), m_owned(true) {}","m_data(0) { m_owned = true; }")],
]
for combo in itertools.product(*knobs):
    s=base
    for o,n in combo:
        if o: 
            if o not in s: continue
            s=s.replace(o,n)
    if 'list_node(list_node* prev' in s: s=s.replace("m_head(T())","m_head(&m_head, &m_head, T())")
    r=score.real('/home/kyle/workspace/moh-frontline/config/GMFE69/scene_dtor.json',s,'__ct__6CSceneFv','800dc8c4',900)
    print([combo.index(c) if False else knobs[i].index(c) for i,c in enumerate(combo)],r)
