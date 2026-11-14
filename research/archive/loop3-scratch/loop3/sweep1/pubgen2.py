import itertools
base=open('pub0.cpp').read()
s=base.index('void CPublisher::PublishEvent'); e=base.index('namespace dwi {\ntemplate <class K, class V>\n__declspec(weak) void hash_map<K, V>::erase')
def fn(top, cond, loop1, loop2, ev='event', sj='subject'):
    return f'''void CPublisher::PublishEvent(ISubject& subject, ESubjectEvent event) {{
{top}    if ({cond})
        return;
    if ({ev} == 3) {{
{loop1}    }} else {{
{loop2}    }}
}}

'''
L1='''        dwi::hash_map<ISubject*, SItem>::iterator it;
        while ((it = sm_pThePublisher->m_map.find(&SJ)) != sm_pThePublisher->m_map.end()) {
            SItem& item = it->second;
            IObserver& observer = *item.m_observer;
            SubjectCallback callback = item.m_callback;
            if (&SJ == *item.m_slot) {
                callback(observer, item.m_slot, EV);
                sm_pThePublisher->m_map.erase(it);
            }
        }
'''
L2w='''        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&SJ);
        while (it != sm_pThePublisher->m_map.end()) {
            SItem& item = it->second;
            IObserver& observer = *item.m_observer;
            SubjectCallback callback = item.m_callback;
            if (BOOL(&SJ == *item.m_slot))
                callback(observer, item.m_slot, EV);
            it = sm_pThePublisher->m_map.find_next(it);
        }
'''
L2f='''        dwi::hash_map<ISubject*, SItem>::iterator it;
        SItem* item;
        for (it = sm_pThePublisher->m_map.find(&SJ), item = &it->second; it != sm_pThePublisher->m_map.end(); it = sm_pThePublisher->m_map.find_next(it), item = &it->second) {
            IObserver& observer = *item->m_observer;
            SubjectCallback callback = item->m_callback;
            if (BOOL(&SJ == *item->m_slot))
                callback(observer, item->m_slot, EV);
        }
'''
tops={'none':('','subject','event'),
 'cev':('    ESubjectEvent const ev = event;\n','subject','ev'),
 'csj':('    ISubject* const sj = &subject;\n','*sj','event'),
 'both':('    ESubjectEvent const ev = event;\n    ISubject* const sj = &subject;\n','*sj','ev'),
 'both2':('    ISubject* const sj = &subject;\n    ESubjectEvent const ev = event;\n','*sj','ev'),
 'iev':('    int const ev = event;\n','subject','(ESubjectEvent)ev'),
}
conds={'plain':'sm_pThePublisher->m_map.m_count == 0','bool':'!(bool)sm_pThePublisher->m_map.m_count'}
n=0
for (tk,(top,sj,ev)),(ck,cond),(lk,l2),bk in itertools.product(tops.items(),conds.items(),{'w':L2w,'f':L2f}.items(),['','(bool)']):
    l1=L1.replace('SJ',sj).replace('EV',ev); l2b=l2.replace('SJ',sj).replace('EV',ev).replace('BOOL',bk)
    t=base[:s]+fn(top,cond,l1,l2b,ev if 'ev'!=ev else ev,sj)+base[e:]
    t=t.replace('if (event == 3)','if (%s == 3)'%ev)
    open(f'pv_{tk}_{ck}_{lk}_{"b" if bk else "n"}.cpp','w').write(t); n+=1
print(n)
