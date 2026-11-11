// bsmessage.cpp: its zero-initialised globals (0x9020 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct MessageView { unsigned char data[0x24]; }; // view: size from the original
struct BSMessageRegistration_struct;

MessageView g_MessageQueue[1024]; // file-local in the original // address: g_MessageQueue 0x80198098
BSMessageRegistration_struct** g_MessageRegistrationArray; // file-local in the original // address: g_MessageRegistrationArray 0x8034e818
BSMessageRegistration_struct* g_MessageRegistrationFreeList; // file-local in the original // address: g_MessageRegistrationFreeList 0x8034e81c
BSMessageRegistration_struct* g_MessageRegistrationFreeListHead; // file-local in the original // address: g_MessageRegistrationFreeListHead 0x8034e820
int g_MessageQueueHead; // file-local in the original // address: g_MessageQueueHead 0x8034e824
int g_MessageQueueTail; // file-local in the original // address: g_MessageQueueTail 0x8034e828
int g_iBSMessageRegistrationListSize;
unsigned char* g_pBSMessageMemory; // file-local in the original // address: g_pBSMessageMemory 0x8034e830
int g_pBSMessageMemoryOffset; // file-local in the original // address: g_pBSMessageMemoryOffset 0x8034e834
