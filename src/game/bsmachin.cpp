// bsmachin.cpp: the behaviour-script virtual machine (the opcode handlers,
// thread creation, execution and destruction, the machine's set-up and
// shutdown), its memory requirement and the property-data readers
// (LoadPropertyData and the per-structure _LoadPropertyData overloads), which
// return a field of a level object's property structure by index. The
// functions and structures are named by the mangled symbols; the structure
// members are inferred from the accessed offsets, and the constants are
// entries of the file's .sdata2 pool. Structure and descriptive names follow
// the Rising Sun reconstruction (EA's later revision); only accessed fields
// and strides are established.


struct xyzProperty_Struct {
    unsigned char unknown00[12];
    int field0c;
};

// Property-data readers. Field names give the byte offset; only the
// accessed members of each structure are established.
struct MOH_fogParams_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    short field70;
    short field72;
    short field74;
    unsigned char unknown76[2];
    int field78;
    int field7c;
};
struct MOH_animatedLight_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char unknown71[1];
    short field72;
};
struct MOH_playerPathController_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    short field72;
    short field74;
    unsigned char unknown76[2];
    int field78;
    int field7c;
};
struct MOH_cameraShaker_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char unknown72[2];
    int field74;
    int field78;
    int field7c;
    int field80;
    int field84;
};
struct MOH_projectileGeneratorTarget_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    short field72;
    int field74;
    int field78;
    int field7c;
    int field80;
    int field84;
    int field88;
};
struct MOH_projectileGenerator_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    short field72;
    int field74;
    int field78;
    int field7c;
    int field80;
    int field84;
    int field88;
    int field8c;
    int field90;
    int field94;
};
struct MOH_particle_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char field72;
    unsigned char field73;
    unsigned char field74;
    unsigned char field75;
    unsigned char field76;
    unsigned char field77;
    unsigned char field78;
    unsigned char field79;
    unsigned char field7a;
    unsigned char field7b;
    unsigned char field7c;
    unsigned char field7d;
    unsigned char field7e;
    unsigned char field7f;
    unsigned char field80;
    unsigned char unknown81[1];
    short field82;
    short field84;
    unsigned char unknown86[2];
    int field88;
    int field8c;
    int field90;
    int field94;
    int field98;
    int field9c;
    int fielda0;
    int fielda4;
    int fielda8;
    int fieldac;
    int fieldb0;
    int fieldb4;
    int fieldb8;
    int fieldbc;
    int fieldc0;
    int fieldc4;
    int fieldc8;
    int fieldcc;
    int fieldd0;
    int fieldd4;
    int fieldd8;
    int fielddc;
    int fielde0;
    int fielde4;
    int fielde8;
};
struct MOH_mechanicEnvMod_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char unknown72[2];
    int field74;
    int field78;
};
struct MOH_envMod_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    int field70;
};
struct MOH_pathPoint_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
};
struct MOH_wayPoint_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
};
struct MOH_powerup_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
};
struct MOH_enemy_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char field72;
    unsigned char field73;
    unsigned char field74;
    unsigned char field75;
    unsigned char field76;
    unsigned char field77;
    unsigned char field78;
    unsigned char field79;
    unsigned char field7a;
    unsigned char field7b;
    unsigned char field7c;
    unsigned char unknown7d[1];
    short field7e;
    short field80;
    short field82;
    short field84;
    short field86;
    short field88;
    short field8a;
    short field8c;
    short field8e;
    int field90;
    int field94;
    int field98;
    int field9c;
    int fielda0;
    int fielda4;
    int fielda8;
    int fieldac;
};
struct MOH_mechanic_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
    unsigned char unknown54[28];
    unsigned char field70;
    unsigned char field71;
    unsigned char unknown72[2];
    int field74;
};
struct MOH_core_Struct {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[10];
    int field48;
    unsigned char unknown4c[4];
    int field50;
};


int _LoadPropertyData(MOH_fogParams_Struct* object, int field);
int _LoadPropertyData(MOH_animatedLight_Struct* object, int field);
int _LoadPropertyData(MOH_playerPathController_Struct* object, int field);
int _LoadPropertyData(MOH_cameraShaker_Struct* object, int field);
int _LoadPropertyData(MOH_projectileGeneratorTarget_Struct* object, int field);
int _LoadPropertyData(MOH_projectileGenerator_Struct* object, int field);
int _LoadPropertyData(MOH_particle_Struct* object, int field);
int _LoadPropertyData(MOH_mechanicEnvMod_Struct* object, int field);
int _LoadPropertyData(MOH_envMod_Struct* object, int field);
int _LoadPropertyData(MOH_pathPoint_Struct* object, int field);
int _LoadPropertyData(MOH_wayPoint_Struct* object, int field);
int _LoadPropertyData(MOH_powerup_Struct* object, int field);
int _LoadPropertyData(MOH_enemy_Struct* object, int field);
int _LoadPropertyData(MOH_mechanic_Struct* object, int field);
int _LoadPropertyData(MOH_core_Struct* object, int field);

int LoadPropertyData(xyzProperty_Struct* properties, int field);

// Only the accessed members are established.
struct TriggerObject_struct {
    unsigned char unknown00[4];
    xyzProperty_Struct* properties;
};

struct BSScriptClassView {
    unsigned char unknown00[96];
    int* sharedValues;
};

class BSObjectUserView;

struct BSObject {
    BSScriptClassView* scriptClass;
    unsigned char unknown04[4];
    TriggerObject_struct* nativeObject;
    BSObjectUserView* user;
    int queueIdentity;
};

struct BSClass_struct;

// Only the accessed members and strides are established.
struct BSClassListView {
    unsigned char unknown00[4];
    BSClass_struct* next;
};

struct BSCode_MessageHandlerEntry_struct {
    unsigned char unknown00[12];
};

struct BSMessageRegistration_struct;

struct BSEventView {
    unsigned int code;
    unsigned short eventNumber;
    unsigned char unknown06;
    unsigned char flags;
};

struct BSStateView {
    unsigned int code;
    BSEventView* events;
    BSCode_MessageHandlerEntry_struct* messages;
    unsigned short parent;
    unsigned char depth;
    unsigned char messageCount;
    unsigned char eventCount;
};

struct BSMessageRegistrationView {
    unsigned char unknown00[22];
    unsigned short flags;
};

struct BSCode_struct {
    int** codeBases;
    unsigned char unknown04[4];
    BSStateView** states;
    unsigned char unknown0c[10];
    unsigned short initialState;
};

struct BSMessageListView {
    BSMessageRegistration_struct* handler;
    BSMessageRegistration_struct* registration;
    BSMessageListView* next;
    unsigned short depth;
    unsigned short stateId;
};

struct BSMachineThread_struct {
    int* frame;
    int* stackTop;
    void* context;
    int* instruction;
    BSStateView* state;
    BSCode_struct* level;
    BSMessageListView* messages;
    unsigned short stateId;
    unsigned short flags;
};

// The object at BSObject+12; only the third virtual slot is called here, and
// its name is unknown.
class BSObjectUserView {
    unsigned char unknown00[12];

public:
    virtual void unknownVirtual0();
    virtual void unknownVirtual1();
    virtual void* unknownVirtual2();
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[12];
};

extern int g_iBSMessageRegistrationListSize;
extern BSClass_struct* g_pcClassList;
extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;


int* g_piStackTop;
int* g_piStackBase;
BSObject* g_pBSObject;
static int* g_piIP;
static int* g_piFrame;
static int g_iStopThread;
static int** g_piCodeBase;
static int* g_piGlobals;
static BSStateView** g_ppsteStateEntries;
BSMachineThread_struct* g_pbmtThread;
static void* g_pObj;
static BSMessageListView* g_MessageList;
static BSMessageListView* g_MessageListHead;
static unsigned char* g_pBSMachineMemory;
static int g_pBSMachineMemoryOffset;
static int g_iDefaultStackSize = 64;
static void (*g_bocfOpCodeFuncs[33])();

void BSMessageRemoveHandler(BSMessageRegistration_struct*);
BSMessageRegistration_struct* BSRegisterMessage(BSCode_MessageHandlerEntry_struct*, BSObject*, unsigned short,
                                                BSMachineThread_struct*, BSMessageRegistration_struct**);
void BSFileDeleteClass(BSClass_struct*);
void* BSUtilGetMemory(int, int);

static BSMessageListView* BSMachineGetFreeMessageList() {
    BSMessageListView* entry = g_MessageListHead;
    if (entry) {
        g_MessageListHead = entry->next;
    }
    return entry;
}

// The signed argument occupies the high half of the instruction word.
inline int BSInstructionArgument() {
    return *g_piIP >> 16;
}

void BSOpCodeFunc_NEXTSTATE() {
    int registerMessages;
    BSMachineThread_struct *thread;
    BSStateView **states;
    int nextState;
    int depth;
    bool foundExit;
    BSEventView *event;
    unsigned short state;
    bool leaveStates;
    BSStateView *stateEntry;
    int eventCount;
    int i;
    BSMessageListView **link;
    BSMessageListView *entry;
    BSMessageListView *next;
    BSCode_MessageHandlerEntry_struct *message;
    int count;
    int j;
    unsigned int code;
    registerMessages = 1;
    thread = g_pbmtThread;
    states = thread->level->states;
    nextState = static_cast<unsigned short>(*g_piStackTop);
    depth = g_ppsteStateEntries[nextState]->depth;
    foundExit = false;
    leaveStates = depth <= g_ppsteStateEntries[thread->stateId]->depth;
    event = 0;
    if (!(thread->flags & 1) && leaveStates) {
        state = thread->stateId;
        while (state != 0xffff && !foundExit) {
            stateEntry = states[state];
            if (stateEntry->depth < depth)
                break;
            eventCount = stateEntry->eventCount;
            event = stateEntry->events;
            for (i = 0; i < eventCount; ++event, ++i) {
                if (event->eventNumber == 1) {
                    if (event->flags & 1)
                        foundExit = true;
                    break;
                }
                if (event->eventNumber > 1)
                    break;
            }
            state = stateEntry->parent;
        }
    }
    if (!foundExit) {
        thread->flags &= ~1;
        if (thread->stateId != nextState) {
            if (leaveStates) {
                link = &thread->messages;
                entry = thread->messages;
                while (entry) {
                    next = entry->next;
                    if (entry->depth > (unsigned int)depth
                            || (entry->depth == (unsigned int)depth && entry->stateId != nextState)) {
                        *link = entry->next;
                        BSMessageRemoveHandler(entry->handler);
                        entry->next = g_MessageListHead;
                        g_MessageListHead = entry;
                        if (entry->registration)
                            reinterpret_cast<BSMessageRegistrationView*>(entry->registration)->flags &= 0xfffd;
                    } else {
                        if (entry->stateId == nextState)
                            registerMessages = 0;
                        link = &entry->next;
                    }
                    entry = next;
                }
            } else {
                registerMessages = 1;
            }
            thread->stateId = nextState;
            thread->state = g_ppsteStateEntries[nextState];
            if (registerMessages) {
                message = thread->state->messages;
                count = thread->state->messageCount;
                state = nextState;
                for (j = 0; j < count; ++j) {
                    BSMessageRegistration_struct *registration;
                    BSMessageRegistration_struct *handler = BSRegisterMessage(
                        &message[j], g_pBSObject, state, thread, &registration);
                    entry = BSMachineGetFreeMessageList();
                    entry->next = thread->messages;
                    entry->registration = registration;
                    entry->handler = handler;
                    entry->depth = depth;
                    entry->stateId = nextState;
                    thread->messages = entry;
                }
            }
        }
        code = thread->state->code;
        g_piIP = g_piCodeBase[code >> 24] + (code & 0x00ffffff);
        g_piStackTop = g_piFrame - 1;
    } else {
        thread->flags |= 1;
        g_piStackTop = g_piFrame + 1;
        code = event->code;
        *reinterpret_cast<unsigned int *>(g_piStackTop - 1) = nextState;
        *reinterpret_cast<BSObject **>(g_piStackTop) = g_pBSObject;
        g_piIP = g_piCodeBase[code >> 24] + (code & 0x00ffffff);
    }
}

void BSOpCodeFunc_TRACE() {
    ++g_piIP;
}

void BSOpCodeFunc_BREAK() {
    g_iStopThread = 1;
    g_piIP += BSInstructionArgument();
}

void BSOpCodeFunc_RETURN() {
    int* previousFrame = g_piFrame;
    int* saved = previousFrame + ((unsigned int)*g_piIP >> 24);
    g_piFrame = reinterpret_cast<int*>(saved[0]);
    if ((*g_piIP >> 16) & 0xff) {
        int* previousTop = g_piStackTop;
        g_piStackTop = previousFrame;
        *g_piStackTop = *previousTop;
    } else {
        g_piStackTop = previousFrame - 1;
    }
    g_piIP = reinterpret_cast<int*>(saved[1]);
}

void BSOpCodeFunc_BIFNCALL() {
    g_iCurrentBIFIndex = BSInstructionArgument();
    g_pBuiltInFunctions[g_iCurrentBIFIndex].function(&g_piStackTop, g_pObj);
    ++g_piIP;
}

void BSOpCodeFunc_FNCALL() {
    int* ip = g_piIP;
    int* previousTop = g_piStackTop;
    int argumentCount = (*ip >> 16) & 0xff;
    g_piStackTop = previousTop + 2;
    previousTop[1] = reinterpret_cast<int>(g_piFrame);
    previousTop[2] = reinterpret_cast<int>(ip + 2);
    g_piFrame = previousTop - argumentCount + 1;
    unsigned int target = ip[1];
    g_piIP = g_piCodeBase[target >> 24] + (target & 0x00ffffff);
}

void BSOpCodeFunc_GP() {
    unsigned int instruction = *g_piIP;
    int field = (instruction >> 16) & 0xff;
    xyzProperty_Struct* properties;
    if (instruction >> 24)
        properties = reinterpret_cast<TriggerObject_struct*>(g_piFrame[3])->properties;
    else
        properties = g_pBSObject->nativeObject->properties;
    ++g_piStackTop;
    *g_piStackTop = LoadPropertyData(properties, field);
    ++g_piIP;
}

void BSOpCodeFunc_LS() {
    int value = g_pBSObject->scriptClass->sharedValues[BSInstructionArgument()];
    *++g_piStackTop = value;
    ++g_piIP;
}

void BSOpCodeFunc_LM() {
    *g_piStackTop = reinterpret_cast<int*>(*g_piStackTop)[BSInstructionArgument()];
    ++g_piIP;
}

void BSOpCodeFunc_SM() {
    reinterpret_cast<int*>(*g_piStackTop)[BSInstructionArgument()] = g_piStackTop[-1];
    g_piStackTop -= 2;
    ++g_piIP;
}

void BSOpCodeFunc_LG() {
    *++g_piStackTop = g_piGlobals[BSInstructionArgument()];
    ++g_piIP;
}

void BSOpCodeFunc_SG() {
    int value = *g_piStackTop--;
    g_piGlobals[BSInstructionArgument()] = value;
    ++g_piIP;
}

void BSOpCodeFunc_LW() {
    *++g_piStackTop = g_piFrame[BSInstructionArgument()];
    ++g_piIP;
}

void BSOpCodeFunc_SW() {
    int value = *g_piStackTop--;
    g_piFrame[BSInstructionArgument()] = value;
    ++g_piIP;
}

void BSOpCodeFunc_PUSH() {
    *++g_piStackTop = g_piIP[1];
    g_piIP += 2;
}

void BSOpCodeFunc_POP() {
    g_piStackTop -= BSInstructionArgument();
    ++g_piIP;
}

void BSOpCodeFunc_CAST() {
    union {
        float value;
        int bits;
    } result;
    int* top = g_piStackTop;
    switch (BSInstructionArgument()) {
    case 2:
        *top = static_cast<int>(*reinterpret_cast<float*>(top));
        break;
    case 3:
        result.value = static_cast<float>(*top);
        *top = result.bits;
        break;
    }
    ++g_piIP;
}

void BSOpCodeFunc_NE() {
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] != g_piStackTop[0];
        break;
    case 0x0203:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) != static_cast<float>(g_piStackTop[0]);
        break;
    case 0x0302:
        g_piStackTop[-1] = static_cast<float>(g_piStackTop[-1]) != *reinterpret_cast<float*>(g_piStackTop);
        break;
    case 0x0303:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) != *reinterpret_cast<float*>(g_piStackTop);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_EQ() {
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] == g_piStackTop[0];
        break;
    case 0x0203:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) == static_cast<float>(g_piStackTop[0]);
        break;
    case 0x0302:
        g_piStackTop[-1] = static_cast<float>(g_piStackTop[-1]) == *reinterpret_cast<float*>(g_piStackTop);
        break;
    case 0x0303:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) == *reinterpret_cast<float*>(g_piStackTop);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_LTE() {
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] <= g_piStackTop[0];
        break;
    case 0x0203:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) <= static_cast<float>(g_piStackTop[0]);
        break;
    case 0x0302:
        g_piStackTop[-1] = static_cast<float>(g_piStackTop[-1]) <= *reinterpret_cast<float*>(g_piStackTop);
        break;
    case 0x0303:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) <= *reinterpret_cast<float*>(g_piStackTop);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_LT() {
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] < g_piStackTop[0];
        break;
    case 0x0203:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) < static_cast<float>(g_piStackTop[0]);
        break;
    case 0x0302:
        g_piStackTop[-1] = static_cast<float>(g_piStackTop[-1]) < *reinterpret_cast<float*>(g_piStackTop);
        break;
    case 0x0303:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) < *reinterpret_cast<float*>(g_piStackTop);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_GTE() {
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] >= g_piStackTop[0];
        break;
    case 0x0203:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) >= static_cast<float>(g_piStackTop[0]);
        break;
    case 0x0302:
        g_piStackTop[-1] = static_cast<float>(g_piStackTop[-1]) >= *reinterpret_cast<float*>(g_piStackTop);
        break;
    case 0x0303:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) >= *reinterpret_cast<float*>(g_piStackTop);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_GT() {
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] > g_piStackTop[0];
        break;
    case 0x0203:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) > static_cast<float>(g_piStackTop[0]);
        break;
    case 0x0302:
        g_piStackTop[-1] = static_cast<float>(g_piStackTop[-1]) > *reinterpret_cast<float*>(g_piStackTop);
        break;
    case 0x0303:
        g_piStackTop[-1] = *reinterpret_cast<float*>(g_piStackTop - 1) > *reinterpret_cast<float*>(g_piStackTop);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_NOT() {
    *g_piStackTop = !*g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_OR() {
    *reinterpret_cast<unsigned int*>(g_piStackTop - 1) = g_piStackTop[-1] || *g_piStackTop;
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_AND() {
    *reinterpret_cast<unsigned int*>(g_piStackTop - 1) = g_piStackTop[-1] && *g_piStackTop;
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_NEG() {
    union {
        float value;
        int bits;
    } result;
    switch ((unsigned int)*g_piIP >> 16) {
    case 2:
        *g_piStackTop = -*g_piStackTop;
        break;
    case 3:
        result.value = -*reinterpret_cast<float*>(g_piStackTop);
        *g_piStackTop = result.bits;
        break;
    default:
        *g_piStackTop = 0;
        break;
    }
    ++g_piIP;
}

void BSOpCodeFunc_MULT() {
    float result = 0.0f;
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] * g_piStackTop[0];
        break;
    case 0x0203:
        result = *reinterpret_cast<float*>(g_piStackTop - 1) * static_cast<float>(g_piStackTop[0]);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    case 0x0302:
        result = static_cast<float>(g_piStackTop[-1]) * *reinterpret_cast<float*>(g_piStackTop);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    case 0x0303:
        result = *reinterpret_cast<float*>(g_piStackTop - 1) * *reinterpret_cast<float*>(g_piStackTop);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_DIV() {
    float result = 0.0f;
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] / g_piStackTop[0];
        break;
    case 0x0203:
        result = *reinterpret_cast<float*>(g_piStackTop - 1) / static_cast<float>(g_piStackTop[0]);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    case 0x0302:
        result = static_cast<float>(g_piStackTop[-1]) / *reinterpret_cast<float*>(g_piStackTop);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    case 0x0303:
        result = *reinterpret_cast<float*>(g_piStackTop - 1) / *reinterpret_cast<float*>(g_piStackTop);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_SUB() {
    float result = 0.0f;
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] - g_piStackTop[0];
        break;
    case 0x0203:
        result = *reinterpret_cast<float*>(g_piStackTop - 1) - static_cast<float>(g_piStackTop[0]);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    case 0x0302:
        result = static_cast<float>(g_piStackTop[-1]) - *reinterpret_cast<float*>(g_piStackTop);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    case 0x0303:
        result = *reinterpret_cast<float*>(g_piStackTop - 1) - *reinterpret_cast<float*>(g_piStackTop);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_ADD() {
    float result = 0.0f;
    switch ((unsigned int)*g_piIP >> 16) {
    case 0x0202:
        g_piStackTop[-1] = g_piStackTop[-1] + g_piStackTop[0];
        break;
    case 0x0203:
        result = *reinterpret_cast<float*>(g_piStackTop - 1) + static_cast<float>(g_piStackTop[0]);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    case 0x0302:
        result = static_cast<float>(g_piStackTop[-1]) + *reinterpret_cast<float*>(g_piStackTop);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    case 0x0303:
        result = *reinterpret_cast<float*>(g_piStackTop - 1) + *reinterpret_cast<float*>(g_piStackTop);
        g_piStackTop[-1] = *reinterpret_cast<int*>(&result);
        break;
    default:
        g_piStackTop[-1] = 0;
        break;
    }
    --g_piStackTop;
    ++g_piIP;
}

void BSOpCodeFunc_JMP() {
    g_piIP += BSInstructionArgument();
}

void BSOpCodeFunc_BNE() {
    if (*g_piStackTop)
        ++g_piIP;
    else
        g_piIP += BSInstructionArgument();
    --g_piStackTop;
}

int BSExecuteThread(BSObject* object, BSMachineThread_struct* thread) {
    BSObject* savedObject = g_pBSObject;
    BSMachineThread_struct* savedThread = g_pbmtThread;
    int** savedCodeBase;
    int* savedStackTop;
    int* savedFrame;
    int* savedIP;
    int* savedGlobals;
    int savedBIFIndex;
    BSStateView** savedStates;
    void* savedObj;

    g_pBSObject = object;
    savedCodeBase = g_piCodeBase;
    g_pbmtThread = thread;
    savedStackTop = g_piStackTop;
    savedFrame = g_piFrame;
    savedIP = g_piIP;
    g_piCodeBase = thread->level->codeBases;
    savedGlobals = g_piGlobals;
    g_piStackTop = thread->stackTop;
    savedBIFIndex = g_iCurrentBIFIndex;
    savedObj = g_pObj;
    g_piFrame = thread->frame;
    g_piIP = thread->instruction;
    savedStates = g_ppsteStateEntries;
    g_piGlobals = reinterpret_cast<int*>(thread->context);
    g_ppsteStateEntries = thread->level->states;
    g_pObj = object->user ? object->user->unknownVirtual2() : 0;
    g_iStopThread = 0;
    while (!g_iStopThread) {
        g_bocfOpCodeFuncs[*g_piIP & 0xff]();
    }
    thread->instruction = g_piIP;
    thread->flags &= ~1;
    g_piStackTop = savedStackTop;
    g_piIP = savedIP;
    g_piFrame = savedFrame;
    g_piCodeBase = savedCodeBase;
    g_iCurrentBIFIndex = savedBIFIndex;
    g_piGlobals = savedGlobals;
    g_ppsteStateEntries = savedStates;
    g_pbmtThread = savedThread;
    g_pObj = savedObj;
    g_pBSObject = savedObject;
    g_iStopThread = 0;
    return 0;
}

int BSDestroyThread(BSMachineThread_struct* thread) {
    BSMessageListView* entry = thread->messages;
    while (entry) {
        BSMessageListView* next = entry->next;
        BSMessageRemoveHandler(entry->handler);
        entry->next = g_MessageListHead;
        g_MessageListHead = entry;
        entry = next;
    }
    return 0;
}

int BSCreateThread(BSObject* object, BSCode_struct* level, BSMachineThread_struct* thread, void* context) {
    thread->level = level;
    thread->stackTop = g_piStackTop + 1;
    thread->frame = thread->stackTop + 1;
    thread->flags = 0;
    thread->context = context;
    thread->stateId = level->initialState;
    thread->state = thread->level->states[thread->stateId];
    unsigned int code = thread->state->code;
    thread->instruction = level->codeBases[code >> 24] + (code & 0x00ffffff);
    thread->messages = 0;
    BSCode_MessageHandlerEntry_struct* message = thread->state->messages;
    int count = thread->state->messageCount;
    for (int i = 0; i < count; ++i) {
        BSMessageRegistration_struct* registration;
        BSMessageRegistration_struct* handler =
            BSRegisterMessage(message, object, thread->stateId, thread, &registration);
        BSMessageListView* entry = BSMachineGetFreeMessageList();
        entry->next = thread->messages;
        entry->registration = registration;
        entry->handler = handler;
        entry->depth = thread->state->depth;
        entry->stateId = thread->stateId;
        thread->messages = entry;
        message++;
    }
    BSExecuteThread(object, thread);
    return 0;
}

int BSEndMachine() {
    BSClass_struct* entry = g_pcClassList;
    while (entry) {
        BSClass_struct* next = reinterpret_cast<BSClassListView*>(entry)->next;
        BSFileDeleteClass(entry);
        entry = next;
    }
    g_pcClassList = 0;
    return 0;
}

// BSInitMachine takes the machine memory (the message lists and the stack) in
// one block, fills the opcode table, links the message lists into the free
// list (an inferred inline helper) and sets up the stack.
inline void LinkMessageList() {
    for (int i = 0; i < g_iBSMessageRegistrationListSize - 1; i++) {
        g_MessageList[i].next = &g_MessageList[i + 1];
    }
}

int BSInitMachine() {
    g_pBSMachineMemory = (unsigned char*)BSUtilGetMemory(
        g_iDefaultStackSize * sizeof(int) + g_iBSMessageRegistrationListSize * sizeof(BSMessageListView), 0);
    g_pBSMachineMemoryOffset = 0;
    g_iStopThread = 0;
    g_pcClassList = 0;
    g_bocfOpCodeFuncs[0] = BSOpCodeFunc_BNE;
    g_bocfOpCodeFuncs[1] = BSOpCodeFunc_JMP;
    g_bocfOpCodeFuncs[2] = BSOpCodeFunc_ADD;
    g_bocfOpCodeFuncs[3] = BSOpCodeFunc_SUB;
    g_bocfOpCodeFuncs[4] = BSOpCodeFunc_DIV;
    g_bocfOpCodeFuncs[5] = BSOpCodeFunc_MULT;
    g_bocfOpCodeFuncs[6] = BSOpCodeFunc_NEG;
    g_bocfOpCodeFuncs[7] = BSOpCodeFunc_EQ;
    g_bocfOpCodeFuncs[30] = BSOpCodeFunc_NE;
    g_bocfOpCodeFuncs[8] = BSOpCodeFunc_LT;
    g_bocfOpCodeFuncs[9] = BSOpCodeFunc_GT;
    g_bocfOpCodeFuncs[10] = BSOpCodeFunc_LTE;
    g_bocfOpCodeFuncs[11] = BSOpCodeFunc_GTE;
    g_bocfOpCodeFuncs[12] = BSOpCodeFunc_AND;
    g_bocfOpCodeFuncs[13] = BSOpCodeFunc_OR;
    g_bocfOpCodeFuncs[14] = BSOpCodeFunc_NOT;
    g_bocfOpCodeFuncs[15] = BSOpCodeFunc_LW;
    g_bocfOpCodeFuncs[16] = BSOpCodeFunc_SW;
    g_bocfOpCodeFuncs[17] = BSOpCodeFunc_PUSH;
    g_bocfOpCodeFuncs[18] = BSOpCodeFunc_POP;
    g_bocfOpCodeFuncs[19] = BSOpCodeFunc_CAST;
    g_bocfOpCodeFuncs[20] = BSOpCodeFunc_FNCALL;
    g_bocfOpCodeFuncs[21] = BSOpCodeFunc_RETURN;
    g_bocfOpCodeFuncs[22] = BSOpCodeFunc_BREAK;
    g_bocfOpCodeFuncs[23] = BSOpCodeFunc_NEXTSTATE;
    g_bocfOpCodeFuncs[24] = BSOpCodeFunc_LG;
    g_bocfOpCodeFuncs[25] = BSOpCodeFunc_SG;
    g_bocfOpCodeFuncs[26] = BSOpCodeFunc_BIFNCALL;
    g_bocfOpCodeFuncs[27] = BSOpCodeFunc_LM;
    g_bocfOpCodeFuncs[28] = BSOpCodeFunc_SM;
    g_bocfOpCodeFuncs[29] = BSOpCodeFunc_GP;
    g_bocfOpCodeFuncs[31] = BSOpCodeFunc_TRACE;
    g_bocfOpCodeFuncs[32] = BSOpCodeFunc_LS;
    g_MessageList = (BSMessageListView*)(g_pBSMachineMemory + g_pBSMachineMemoryOffset);
    g_pBSMachineMemoryOffset += g_iBSMessageRegistrationListSize * sizeof(BSMessageListView);
    LinkMessageList();
    g_MessageList[g_iBSMessageRegistrationListSize - 1].next = 0;
    g_MessageListHead = g_MessageList;
    g_piStackBase = (int*)(g_pBSMachineMemory + g_pBSMachineMemoryOffset);
    g_pBSMachineMemoryOffset += g_iDefaultStackSize * sizeof(int);
    g_piStackTop = g_piStackBase;
    g_piFrame = g_piStackTop + 1;
    return 0;
}

int BSMachineGetMemoryRequirements() {
    return g_iDefaultStackSize * sizeof(int) + g_iBSMessageRegistrationListSize * sizeof(BSMessageListView);
}

int LoadPropertyData(xyzProperty_Struct* properties, int field) {
    switch (properties->field0c) {
    case 1:
        return _LoadPropertyData(reinterpret_cast<MOH_core_Struct*>(properties), field);
    case 2:
        return _LoadPropertyData(reinterpret_cast<MOH_mechanic_Struct*>(properties), field);
    case 3:
        return _LoadPropertyData(reinterpret_cast<MOH_enemy_Struct*>(properties), field);
    case 4:
        return _LoadPropertyData(reinterpret_cast<MOH_powerup_Struct*>(properties), field);
    case 5:
        return _LoadPropertyData(reinterpret_cast<MOH_wayPoint_Struct*>(properties), field);
    case 6:
        return _LoadPropertyData(reinterpret_cast<MOH_pathPoint_Struct*>(properties), field);
    case 7:
        return _LoadPropertyData(reinterpret_cast<MOH_envMod_Struct*>(properties), field);
    case 8:
        return _LoadPropertyData(reinterpret_cast<MOH_mechanicEnvMod_Struct*>(properties), field);
    case 9:
        return _LoadPropertyData(reinterpret_cast<MOH_particle_Struct*>(properties), field);
    case 10:
        return _LoadPropertyData(reinterpret_cast<MOH_projectileGenerator_Struct*>(properties), field);
    case 11:
        return _LoadPropertyData(reinterpret_cast<MOH_projectileGeneratorTarget_Struct*>(properties), field);
    case 12:
        return _LoadPropertyData(reinterpret_cast<MOH_cameraShaker_Struct*>(properties), field);
    case 13:
        return _LoadPropertyData(reinterpret_cast<MOH_playerPathController_Struct*>(properties), field);
    case 14:
        return _LoadPropertyData(reinterpret_cast<MOH_animatedLight_Struct*>(properties), field);
    case 15:
        return _LoadPropertyData(reinterpret_cast<MOH_fogParams_Struct*>(properties), field);
    }
    return 0;
}

int _LoadPropertyData(MOH_fogParams_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field78;
    case 23:
        return object->field7c;
    case 24:
        return object->field70;
    case 25:
        return object->field72;
    case 26:
        return object->field74;
    }
    return 0;
}

int _LoadPropertyData(MOH_animatedLight_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 24:
        return object->field70;
    case 25:
        return object->field72;
    }
    return 0;
}

int _LoadPropertyData(MOH_playerPathController_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field7c;
    case 24:
        return object->field71;
    case 25:
        return object->field72;
    case 26:
        return object->field74;
    case 27:
        return object->field78;
    }
    return 0;
}

int _LoadPropertyData(MOH_cameraShaker_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field84;
    case 24:
        return object->field71;
    case 25:
        return object->field74;
    case 26:
        return object->field78;
    case 27:
        return object->field7c;
    case 28:
        return object->field80;
    }
    return 0;
}

int _LoadPropertyData(MOH_projectileGeneratorTarget_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field88;
    case 24:
        return object->field71;
    case 25:
        return object->field72;
    case 26:
        return object->field74;
    case 27:
        return object->field78;
    case 28:
        return object->field7c;
    case 29:
        return object->field80;
    case 30:
        return object->field84;
    }
    return 0;
}

int _LoadPropertyData(MOH_projectileGenerator_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field90;
    case 24:
        return object->field71;
    case 25:
        return object->field72;
    case 26:
        return object->field74;
    case 27:
        return object->field78;
    case 28:
        return object->field7c;
    case 29:
        return object->field80;
    case 30:
        return object->field94;
    case 31:
        return object->field84;
    case 32:
        return object->field88;
    case 33:
        return object->field8c;
    }
    return 0;
}

int _LoadPropertyData(MOH_particle_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 23:
        return object->field82;
    case 24:
        return object->field84;
    case 25:
        return object->field88;
    case 26:
        return object->field8c;
    case 27:
        return object->field90;
    case 28:
        return object->field94;
    case 29:
        return object->field98;
    case 30:
        return object->field9c;
    case 31:
        return object->fielda0;
    case 32:
        return object->fielda4;
    case 33:
        return object->fielda8;
    case 34:
        return object->fieldac;
    case 35:
        return object->fieldb0;
    case 36:
        return object->fieldb4;
    case 37:
        return object->fieldb8;
    case 38:
        return object->fieldbc;
    case 39:
        return object->fieldc0;
    case 40:
        return object->fieldc4;
    case 41:
        return object->fieldc8;
    case 42:
        return object->field71;
    case 43:
        return object->field72;
    case 44:
        return object->field73;
    case 45:
        return object->field74;
    case 46:
        return object->field75;
    case 47:
        return object->field76;
    case 48:
        return object->field77;
    case 49:
        return object->field78;
    case 50:
        return object->field79;
    case 51:
        return object->fieldcc;
    case 52:
        return object->fieldd0;
    case 53:
        return object->fieldd4;
    case 54:
        return object->fieldd8;
    case 55:
        return object->fielddc;
    case 56:
        return object->fielde0;
    case 57:
        return object->fielde4;
    case 58:
        return object->fielde8;
    case 59:
        return object->field7a;
    case 60:
        return object->field7b;
    case 61:
        return object->field7c;
    case 62:
        return object->field7d;
    case 63:
        return object->field7e;
    case 64:
        return object->field7f;
    case 65:
        return object->field80;
    case 66:
        return object->field70 & 1;
    }
    return 0;
}

int _LoadPropertyData(MOH_mechanicEnvMod_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field74;
    case 24:
        return object->field71;
    case 25:
        return object->field78;
    }
    return 0;
}

int _LoadPropertyData(MOH_envMod_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    }
    return 0;
}

int _LoadPropertyData(MOH_pathPoint_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    }
    return 0;
}

int _LoadPropertyData(MOH_wayPoint_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70 & 1;
    case 23:
        return (object->field70 >> 1) & 1;
    case 24:
        return (object->field70 >> 2) & 1;
    case 25:
        return (object->field70 >> 3) & 1;
    case 26:
        return (object->field70 >> 4) & 1;
    case 27:
        return (object->field70 >> 5) & 1;
    case 28:
        return (object->field70 >> 6) & 1;
    case 29:
        return (object->field70 >> 7) & 1;
    }
    return 0;
}

int _LoadPropertyData(MOH_powerup_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    }
    return 0;
}

int _LoadPropertyData(MOH_enemy_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field90;
    case 24:
        return object->field71;
    case 25:
        return object->field94;
    case 26:
        return object->field72;
    case 27:
        return object->field98;
    case 28:
        return object->field9c;
    case 29:
        return object->fielda0;
    case 30:
        return object->field73;
    case 31:
        return object->field74;
    case 32:
        return object->field75;
    case 33:
        return object->fielda4;
    case 34:
        return object->fielda8;
    case 35: {
        float value = object->field7e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 36: {
        float value = object->field80 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 37: {
        float value = object->field82 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 38:
        return object->field76;
    case 39:
        return object->field77;
    case 40:
        return object->field78;
    case 41:
        return object->field79;
    case 42:
        return object->field84;
    case 43:
        return object->field7a;
    case 44:
        return object->fieldac;
    case 45: {
        float value = object->field86 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 46: {
        float value = object->field88 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 47: {
        float value = object->field8a / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 48: {
        float value = object->field8c / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 49: {
        float value = object->field8e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 50:
        return object->field7b;
    case 51:
        return object->field7c;
    }
    return 0;
}

int _LoadPropertyData(MOH_mechanic_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    case 22:
        return object->field70;
    case 23:
        return object->field74;
    case 24:
        return object->field71;
    }
    return 0;
}

int _LoadPropertyData(MOH_core_Struct* object, int field) {
    switch (field) {
    case 0:
        return object->field36;
    case 2:
        return object->field38;
    case 4:
        return object->field35;
    case 5:
        return object->field48;
    case 7:
        return object->field50;
    case 8:
        return object->field2c;
    case 9: {
        float value = object->field2e / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 10: {
        float value = object->field30 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 11: {
        float value = object->field32 / 16.0f;
        return *reinterpret_cast<int*>(&value);
    }
    case 18:
        return object->field34 & 1;
    case 19:
        return object->field3a;
    case 20:
        return (object->field34 >> 1) & 1;
    case 21:
        return object->field3c;
    }
    return 0;
}



