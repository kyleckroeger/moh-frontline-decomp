extern "C" int sprintf(char*, const char*, ...);
extern "C" unsigned long strlen(const char*);
extern "C" int stricmp(const char*, const char*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

template <class T> inline void ChangeEndian(T*& value) {
    ChangeEndian(*reinterpret_cast<long*>(&value));
}

struct BSArg {
    void* m_value;
    short m_type;
};

struct BSParam {
    void* m_value;
    void* m_extra;
    short m_type;
};

struct BSOpcode {
    void* m_id;
    BSArg* m_args;
    BSParam* m_params;
    unsigned short m_flags;
    unsigned char unknown0e;
    unsigned char m_numParams;
    unsigned char m_numArgs;
    unsigned char unknown11[3];
};

struct BSState {
    int* m_lists;
    void* m_value04;
    BSOpcode** m_opcodes;
    unsigned short m_numOpcodes;
    unsigned char unknown0e[6];
    unsigned short m_value14;
    unsigned short m_value16;
    unsigned char m_numLists;
    unsigned char unknown19[3];
};

struct BSClass_struct {
    BSState* m_states;
    BSClass_struct* next;
    char name[64];
    long m_value48;
    void* codeFile;
    void* classFile;
    long m_memory;
    unsigned short m_numStates;
    bool loaded;
    signed char m_debug;
    long m_numStrings;
    char** m_strings;
};

extern BSClass_struct* g_pcClassList;

static inline BSClass_struct* BSFileFindClass(const char* name) {
    for (BSClass_struct* entry = g_pcClassList; entry; entry = entry->next) {
        if (stricmp(name, entry->name) == 0)
            return entry;
    }
    return 0;
}
int BSUtilComputeClassMemoryRequirements(BSClass_struct*);

int BSFileReadClass(const char* name, bool, bool, BSClass_struct** result) {
    char path[100];
    int size;
    char message[268];
    unsigned int j;
    sprintf(path, "%s.sin", name);
    char* code = (char*)TLT_LoadFileFromLevelBigFile(path, &size);
    int offset = 200;
    ChangeEndian(*(int*)(code + 0x20));
    *result = (BSClass_struct*)(code + 100);
    BSClass_struct* cls = *result;
    ChangeEndian(cls->m_value48);
    ChangeEndian(cls->m_memory);
    ChangeEndian(*reinterpret_cast<short*>(&cls->m_numStates));
    ChangeEndian(cls->m_numStrings);
    ChangeEndian(cls->m_strings);
    if ((*result)->m_debug != -1)
        sprintf(message, "ERROR:  script %s is a debugging version.  Incompatible with nondebug engine\n", path);
    (*result)->loaded = true;
    (*result)->next = 0;
    (*result)->m_states = 0;
    (*result)->codeFile = code;
    cls = *result;
    if (cls->m_numStrings > 0) {
        cls->m_strings = (char**)(code + (int)cls->m_strings);
        offset = (*result)->m_numStrings * 4 + 200;
        for (int i = 0; i < (*result)->m_numStrings; i++) {
            ChangeEndian(*reinterpret_cast<long*>(&(*result)->m_strings[i]));
            (*result)->m_strings[i] = code + (int)(*result)->m_strings[i];
            unsigned long length = strlen((*result)->m_strings[i]) + 1;
            if (length & 3)
                length += 4 - (length & 3);
            offset += length;
        }
    } else {
        cls->m_strings = 0;
    }
    (*result)->m_states = (BSState*)(code + offset);
    offset += (*result)->m_numStates * sizeof(BSState);
    for (int s = 0; s < (*result)->m_numStates; s++) {
        BSState* state = &(*result)->m_states[s];
        ChangeEndian(state->m_lists);
        ChangeEndian(state->m_value04);
        ChangeEndian(state->m_numOpcodes);
        ChangeEndian(*reinterpret_cast<short*>(&state->m_value14));
        ChangeEndian(*reinterpret_cast<short*>(&state->m_value16));
        state->m_lists = (int*)(code + offset);
        offset += (state->m_numLists + 1) * 4;
        state->m_opcodes = (BSOpcode**)(code + offset);
        offset += state->m_numOpcodes * 4;
        for (j = 0; j < state->m_numOpcodes; j++) {
            ChangeEndian(*reinterpret_cast<long*>(&state->m_opcodes[j]));
            if (state->m_opcodes[j] != (BSOpcode*)-1 && state->m_opcodes[j] != (BSOpcode*)-2) {
                state->m_opcodes[j] = (BSOpcode*)(code + offset);
                offset += sizeof(BSOpcode);
            }
        }
        for (j = 0; j < state->m_numOpcodes; j++) {
            BSOpcode* opcode = state->m_opcodes[j];
            if (opcode != (BSOpcode*)-1 && opcode != (BSOpcode*)-2) {
                ChangeEndian(opcode->m_id);
                ChangeEndian(*reinterpret_cast<short*>(&opcode->m_flags));
                ChangeEndian(opcode->m_args);
                ChangeEndian(opcode->m_params);
                if (state->m_opcodes[j]->m_numArgs) {
                    state->m_opcodes[j]->m_args = (BSArg*)(code + offset);
                    int count = state->m_opcodes[j]->m_numArgs;
                    BSArg* arg = state->m_opcodes[j]->m_args;
                    offset += count * sizeof(BSArg);
                    for (int k = 0; k < count; k++) {
                        ChangeEndian(arg->m_value);
                        ChangeEndian(arg->m_type);
                        arg++;
                    }
                } else {
                    state->m_opcodes[j]->m_args = 0;
                }
            }
        }
        for (j = 0; j < state->m_numOpcodes; j++) {
            BSOpcode* opcode = state->m_opcodes[j];
            if (opcode != (BSOpcode*)-1 && opcode != (BSOpcode*)-2) {
                if (opcode->m_numParams) {
                    opcode->m_params = (BSParam*)(code + offset);
                    int count = state->m_opcodes[j]->m_numParams;
                    BSParam* param = state->m_opcodes[j]->m_params;
                    offset += count * sizeof(BSParam);
                    for (int k = 0; k < count; k++) {
                        ChangeEndian(param->m_value);
                        ChangeEndian(param->m_extra);
                        ChangeEndian(param->m_type);
                        param++;
                    }
                } else {
                    opcode->m_params = 0;
                }
            }
        }
    }
    sprintf(path, "%s.cbs", name);
    char* data = (char*)TLT_LoadFileFromLevelBigFile(path, &size);
    ChangeEndian(*(int*)(data + 0x20));
    BSClass_struct* parent = 0;
    if (data[0x24]) {
        parent = BSFileFindClass(data + 0x24);
        int error;
        if (parent) {
            if (parent->loaded)
                error = 0;
            else
                error = BSFileReadClass(data + 0x24, true, false, &parent);
        } else {
            BSFileReadClass(data + 0x24, true, true, &parent);
            error = 0;
            BSClass_struct* added = parent;
            added->next = g_pcClassList;
            g_pcClassList = added;
        }
        if (error)
            return error;
        for (int s = 0; s < (*result)->m_numStates; s++) {
            BSState* state = &(*result)->m_states[s];
            if (s < parent->m_numStates) {
                BSState* other = &parent->m_states[s];
                for (int j = 0; j < state->m_numOpcodes; j++) {
                    if (state->m_opcodes[j] == (BSOpcode*)-2)
                        state->m_opcodes[j] = other->m_opcodes[j];
                    else if (state->m_opcodes[j] == (BSOpcode*)-1)
                        state->m_opcodes[j] = 0;
                }
            }
        }
    }
    (*result)->classFile = data;
    int count = *(int*)(data + 100);
    offset = 0x68;
    ChangeEndian(count);
    for (int s = 0; s < (int)count; s++) {
        unsigned long lists = *(unsigned long*)(data + offset);
        offset += 4;
        BSState* state = &(*result)->m_states[s];
        unsigned int index = state->m_numLists;
        ChangeEndian(lists);
        if (lists != 0) {
            state->m_lists[index] = (int)(data + offset);
            offset += lists * 4;
            long* list = (long*)(*result)->m_states[s].m_lists[index];
            int n = lists;
            for (int k = 0; k < n; k++)
                ChangeEndian(list[k]);
        } else {
            state->m_lists[index] = 0;
        }
        if (index) {
            if (!parent)
                return 5;
            BSState* base = parent->m_states;
            for (j = 0; j < index; j++)
                (*result)->m_states[s].m_lists[j] = base[s].m_lists[j];
        }
    }
    (*result)->m_memory = BSUtilComputeClassMemoryRequirements(*result);
    return 0;
}
