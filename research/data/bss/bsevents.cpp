// bsevents.cpp: its zero-initialised globals (0x2c bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct EventBucketView;
struct EventRecordView;

EventBucketView* g_pEventHashTable; // file-local in the original // address: g_pEventHashTable 0x8034e7a0
EventRecordView* g_pEventRecordArray; // file-local in the original // address: g_pEventRecordArray 0x8034e7a4
EventRecordView* g_pFreeEventRecordHead; // file-local in the original // address: g_pFreeEventRecordHead 0x8034e7a8
EventRecordView* g_pEventRecordExecutionListHead; // file-local in the original // address: g_pEventRecordExecutionListHead 0x8034e7ac
EventRecordView* g_pEventRecordExecutionListTail; // file-local in the original // address: g_pEventRecordExecutionListTail 0x8034e7b0
EventRecordView* g_pCurrentProcessingEvent; // file-local in the original // address: g_pCurrentProcessingEvent 0x8034e7b4
EventRecordView* g_pImmediateEventQueue; // file-local in the original // address: g_pImmediateEventQueue 0x8034e7b8
EventRecordView* g_pImmediateEventQueueTail; // file-local in the original // address: g_pImmediateEventQueueTail 0x8034e7bc
int g_eProcessState; // file-local in the original // address: g_eProcessState 0x8034e7c0
char* g_pBSEventMemory; // file-local in the original // address: g_pBSEventMemory 0x8034e7c4
int g_pBSEventMemoryOffset; // file-local in the original // address: g_pBSEventMemoryOffset 0x8034e7c8
