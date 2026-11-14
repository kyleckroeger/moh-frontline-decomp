BLOCK='''            SItem& item = it->second;
            IObserver& observer = *item.m_observer;
            if (&subject == *item.m_slot)
                item.m_callback(observer, item.m_slot, event);
            it = sm_pThePublisher->m_map.find_next(it);
'''
IND='            '
S=['SItem& item = it->second;','IObserver& observer = *item.m_observer;','SubjectCallback callback;','callback = item.m_callback;','ISubject** slot;','slot = item.m_slot;',
   'if (&subject == *slot)\n                callback(observer, slot, event);','it = sm_pThePublisher->m_map.find_next(it);']
DEP={0:[],1:[0],2:[],3:[0,2],4:[],5:[0,4],6:[1,3,5],7:[0,1,3,5,6]}
