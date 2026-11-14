T='        m_head.m_next = &m_head; m_head.m_prev = &m_head; m_count = 0; if (m_owned) MEM_free(m_pool); m_pool = 0;'
F='node->m_prev = 0; node->m_next = m_free; m_free = node;'
p(){ python3 pr.py "$@"; }
p c "list_node<T>* node = m_head.m_next; while (node != &m_head) { list_node<T>* next = node->m_next; $F node = next; } $T"
p d "for (list_node<T>* node = m_head.m_next; node != &m_head;) { list_node<T>* next = node->m_next; $F node = next; } $T"
p e "clear(); if (m_owned) MEM_free(m_pool); m_pool = 0;" "void clear() { list_node<T>* next; list_node<T>* node = m_head.m_next; while (node != &m_head) { next = node->m_next; $F node = next; } m_head.m_next = &m_head; m_head.m_prev = &m_head; m_count = 0; }"
p f "clear(); if (m_owned) MEM_free(m_pool); m_pool = 0;" "void clear() { list_node<T>* node = m_head.m_next; list_node<T>* next; while (node != &m_head) { next = node->m_next; $F node = next; } m_head.m_next = &m_head; m_head.m_prev = &m_head; m_count = 0; }"
p g "list_node<T>* node = m_head.m_next; while (node != &m_head) { list_node<T>* next = node->m_next; release(node); node = next; } $T" "void release(list_node<T>* node) { $F }"
p h "list_node<T>* next; list_node<T>* node = m_head.m_next; while (node != &m_head) { next = node->m_next; release(node); node = next; } $T" "void release(list_node<T>* node) { $F }"
p i "list_node<T>* node = m_head.m_next; while (node != &m_head) { node = release(node); } $T" "list_node<T>* release(list_node<T>* node) { list_node<T>* next = node->m_next; $F return next; }"
p j "list_node<T>* node = m_head.m_next; while (node != &m_head) { list_node<T>* cur = node; node = node->m_next; cur->m_prev = 0; cur->m_next = m_free; m_free = cur; } $T"
p k "list_node<T>* node; list_node<T>* next; for (node = m_head.m_next; node != &m_head; node = next) { next = node->m_next; $F } $T"
p l "list_node<T>* next; list_node<T>* node; for (node = m_head.m_next; node != &m_head; node = next) { next = node->m_next; $F } $T"
