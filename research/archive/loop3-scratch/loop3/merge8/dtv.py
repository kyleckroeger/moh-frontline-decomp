"""dtv.py SRC TPL FUNCS...: try ~list body variants (start '~list() {' .. 'm_head.m_next = &m_head;')."""
import sys,subprocess,re,os
src=open(sys.argv[1]).read(); tpl=sys.argv[2]; funcs=sys.argv[3:]
src=src.replace('    ~list() {','    list_node<T>* free_node(list_node<T>* node) {\n list_node<T>* next = node->m_next;\n node->m_prev = 0;\n node->m_next = m_free;\n m_free = node;\n return next;\n }\n    ~list() {',1)
i=src.index('    ~list() {'); j=src.index('        m_head.m_next = &m_head;\n        m_head.m_prev = &m_head;\n        m_count = 0;',i)
tail='''            node->m_prev = 0;
            node->m_next = m_free;
            m_free = node;
'''
V={
'A':'list_node<T>* next;\n list_node<T>* node = m_head.m_next;\n while (node != &m_head) {\n next = node->m_next;\n'+tail+' node = next;\n }\n',
'C':'list_node<T>* node = m_head.m_next;\n while (node != &m_head) {\n list_node<T>* next = node->m_next;\n'+tail+' node = next;\n }\n',
'D':'for (list_node<T>* node = m_head.m_next; node != &m_head;) {\n list_node<T>* next = node->m_next;\n'+tail+' node = next;\n }\n',
'E':'list_node<T>* next;\n list_node<T>* node;\n for (node = m_head.m_next; node != &m_head; node = next) {\n next = node->m_next;\n'+tail+' }\n',
'G':'list_node<T>* node = m_head.m_next;\n list_node<T>* next;\n for (; node != &m_head; node = next) {\n next = node->m_next;\n'+tail+' }\n',
'H':'list_node<T>* node = m_head.m_next;\n while (node != &m_head) {\n list_node<T>* next = node->m_next;\n node->~list_node<T>();\n'+tail+' node = next;\n }\n',
'I':'list_node<T>* next;\n list_node<T>* node = m_head.m_next;\n while (node != &m_head) {\n next = node->m_next;\n node->~list_node<T>();\n'+tail+' node = next;\n }\n',
'K':'list_node<T>* next;\n list_node<T>* node = m_head.m_next;\n while (node != end().m_node) {\n next = node->m_next;\n'+tail+' node = next;\n }\n',
'L':'list_node<T>* node = m_head.m_next;\n list_node<T>* next;\n while (node != end().m_node) {\n next = node->m_next;\n'+tail+' node = next;\n }\n',
'M':'list_node<T>* node = m_head.m_next;\n while (node != &m_head) {\n node = free_node(node);\n }\n',
'N':'list_node<T>* next;\n list_node<T>* node;\n node = m_head.m_next;\n while (node != &m_head) {\n next = node->m_next;\n'+tail+' node = next;\n }\n',
'O':'list_node<T>* node = m_head.m_next;\n list_node<T>* next;\n while (node != &m_head) {\n next = node->m_next;\n node->m_prev = 0;\n node->m_next = m_free;\n m_free = node;\n node = next;\n }\n',
'P':'list_node<T>* next;\n for (list_node<T>* node = m_head.m_next; node != &m_head; node = next) {\n next = node->m_next;\n'+tail+' }\n',
'J':'list_node<T>* node = m_head.m_next;\n while (node != &m_head) {\n list_node<T>* cur = node;\n node = node->m_next;\n cur->m_prev = 0;\n cur->m_next = m_free;\n m_free = cur;\n }\n',
}
for k in (os.environ.get('V') or ''.join(V)).split(',') if os.environ.get('V') else V:
    s=src[:i]+'    ~list() {\n'+V[k]+src[j:]
    p='/tmp/claude-1000/dtv_%d.cpp'%os.getpid(); open(p,'w').write(s)
    o=subprocess.run(['python3','scratch/loop3/merge8/chk.py',tpl,p]+funcs,capture_output=True,text=True).stdout
    print(k, ' '.join(l.strip() for l in o.splitlines() if l.startswith('   ') or l.startswith('functions')))
