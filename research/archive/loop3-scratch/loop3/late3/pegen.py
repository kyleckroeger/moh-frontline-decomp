import itertools,subprocess,sys
s=open('pe0.cpp').read()
a=s.index('    } else {\n        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher')
b=s.index('\n    }\n}\n',a)
L2_its={
 'init':'        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&subject);\n        while (it != sm_pThePublisher->m_map.end()) {\n',
 'late':'        dwi::hash_map<ISubject*, SItem>::iterator it;\n        it = sm_pThePublisher->m_map.find(&subject);\n        while (it != sm_pThePublisher->m_map.end()) {\n',
 'for':'        for (dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&subject); it != sm_pThePublisher->m_map.end(); ) {\n',
}
bodies={
 'cbTop':['SubjectCallback callback = item.m_callback;','SItem& item = it->second;','IObserver& observer = *item.m_observer;'],
 'cbMid':['SItem& item = it->second;','SubjectCallback callback = item.m_callback;','IObserver& observer = *item.m_observer;'],
 'cbLast':['SItem& item = it->second;','IObserver& observer = *item.m_observer;','SubjectCallback callback = item.m_callback;'],
 'cbNoItem':['SubjectCallback callback = it->second.m_callback;','SItem& item = it->second;','IObserver& observer = *item.m_observer;'],
 'cbConst':['SItem& item = it->second;','IObserver& observer = *item.m_observer;','SubjectCallback const callback = item.m_callback;'],
 'cbLate':['SItem& item = it->second;','IObserver& observer = *item.m_observer;','SubjectCallback callback;','callback = item.m_callback;'],
}
n=0
for (ik,iv),(bk,bv) in itertools.product(L2_its.items(),bodies.items()):
    if bk=='cbTop': bv=['SItem& item = it->second;']+[bv[0].replace('item.','it->second.')]+[bv[2]] if False else bv
    body=''.join('            '+x+'\n' for x in bv)
    body+='            if (&subject == *item.m_slot)\n                callback(observer, item.m_slot, event);\n            it = sm_pThePublisher->m_map.find_next(it);\n        }\n'
    if bk=='cbTop': body=body.replace('callback = item.m_callback','callback = it->second.m_callback')
    seg='    } else {\n'+iv+body
    open(f'pg_{ik}_{bk}.cpp','w').write(s[:a]+seg+s[b:])
