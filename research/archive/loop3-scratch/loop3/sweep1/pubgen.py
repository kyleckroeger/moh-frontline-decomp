base=open('pub0.cpp').read()
old=base[base.index('    } else {\n        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher'):base.index('namespace dwi {\ntemplate <class K, class V>\n__declspec(weak) void hash_map<K, V>::erase')]
V={}
V['itpath']='''    } else {
        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&subject);
        while (it != sm_pThePublisher->m_map.end()) {
            SItem& item = it->second;
            IObserver& observer = *item.m_observer;
            if (&subject == *item.m_slot)
                it->second.m_callback(observer, item.m_slot, event);
            it = sm_pThePublisher->m_map.find_next(it);
        }
    }
}

'''
V['late']='''    } else {
        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&subject);
        while (it != sm_pThePublisher->m_map.end()) {
            SubjectCallback callback;
            SItem& item = it->second;
            IObserver& observer = *item.m_observer;
            callback = item.m_callback;
            if (&subject == *item.m_slot)
                callback(observer, item.m_slot, event);
            it = sm_pThePublisher->m_map.find_next(it);
        }
    }
}

'''
V['boolm']='''    } else {
        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&subject);
        while (it != sm_pThePublisher->m_map.end()) {
            SItem& item = it->second;
            IObserver& observer = *item.m_observer;
            bool match = &subject == *item.m_slot;
            if (match)
                item.m_callback(observer, item.m_slot, event);
            it = sm_pThePublisher->m_map.find_next(it);
        }
    }
}

'''
V['forc']='''    } else {
        dwi::hash_map<ISubject*, SItem>::iterator it;
        for (it = sm_pThePublisher->m_map.find(&subject); it != sm_pThePublisher->m_map.end(); it = sm_pThePublisher->m_map.find_next(it)) {
            SItem& item = it->second;
            IObserver& observer = *item.m_observer;
            if (&subject == *item.m_slot)
                item.m_callback(observer, item.m_slot, event);
        }
    }
}

'''
V['itall']='''    } else {
        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&subject);
        while (it != sm_pThePublisher->m_map.end()) {
            IObserver& observer = *it->second.m_observer;
            if (&subject == *it->second.m_slot)
                it->second.m_callback(observer, it->second.m_slot, event);
            it = sm_pThePublisher->m_map.find_next(it);
        }
    }
}

'''
V['slotit']='''    } else {
        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&subject);
        while (it != sm_pThePublisher->m_map.end()) {
            SItem& item = it->second;
            IObserver& observer = *item.m_observer;
            if (&subject == *it->second.m_slot)
                item.m_callback(observer, item.m_slot, event);
            it = sm_pThePublisher->m_map.find_next(it);
        }
    }
}

'''
V['cbcast']='''    } else {
        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&subject);
        while (it != sm_pThePublisher->m_map.end()) {
            SItem& item = it->second;
            IObserver& observer = *item.m_observer;
            SubjectCallback const callback = item.m_callback;
            if (&subject == *item.m_slot)
                callback(observer, item.m_slot, event);
            it = sm_pThePublisher->m_map.find_next(it);
        }
    }
}

'''
V['ptritem']='''    } else {
        dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(&subject);
        while (it != sm_pThePublisher->m_map.end()) {
            SItem* item = &it->second;
            IObserver& observer = *item->m_observer;
            if (&subject == *item->m_slot)
                item->m_callback(observer, item->m_slot, event);
            it = sm_pThePublisher->m_map.find_next(it);
        }
    }
}

'''
for k,v in V.items(): open(f'pv_{k}.cpp','w').write(base.replace(old,v))
