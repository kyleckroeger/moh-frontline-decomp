// The animation system's start-up and shutdown, which register the user-opcode
// handlers with the animation module. CAnimCallbacks is named by the mangled
// symbols. _AnimIntfOpcodeSkip follows them.
extern "C" {
void AnimStShutdown(void);
int AnimWgtShutdown(void);
void AnimShutdown(void);
void AnimInit(void);
int AnimWgtInit(void);
void AnimStInit(void);
int AnimSeqGetOpcodeParameter(void);
int sprintf(char*, const char*, ...);
}
void AnimCallback_Init();

class CAnimCallbacks {
public:
    static int _AnimIntfOpcodeSkip(void*, unsigned short, unsigned long);
    static int _AnimIntfOpcodeProcess(void*, unsigned short, unsigned long);
};

extern "C" void AnimSetUserOpcodeCallbacks(int (*)(void*, unsigned short, unsigned long),
                                           int (*)(void*, unsigned short, unsigned long));

extern "C" void AnimSystem_Shutdown(void) {
    AnimStShutdown();
    AnimWgtShutdown();
    AnimShutdown();
}

extern "C" void AnimSystem_Init(void) {
    AnimInit();
    AnimSetUserOpcodeCallbacks(CAnimCallbacks::_AnimIntfOpcodeProcess, CAnimCallbacks::_AnimIntfOpcodeSkip);
    AnimWgtInit();
    AnimStInit();
    AnimCallback_Init();
}

// The skip handler consumes the parameters of the user opcodes it knows
// (the opcode values and their parameter counts are read from the code; their
// meanings are unknown) and formats a message for any other opcode; the
// message buffer is not used further.
int CAnimCallbacks::_AnimIntfOpcodeSkip(void*, unsigned short opcode, unsigned long) {
    int result = 0;

    switch (opcode) {
    case 0xC900:
        result = 1;
        break;
    case 0xC80D:
    case 0xC80E:
    case 0xC80F:
    case 0xC811:
    case 0xC812:
    case 0xCA00:
    case 0xCA0A:
    case 0xCA13:
        AnimSeqGetOpcodeParameter();
        result = 1;
        break;
    case 0xC80B:
        AnimSeqGetOpcodeParameter();
        AnimSeqGetOpcodeParameter();
        result = 1;
        break;
    case 0xC803:
    case 0xC804:
    case 0xC805:
    case 0xC808:
    case 0xC809:
    case 0xC80C:
        result = 1;
        break;
    case 0xC806:
        result = 0;
        break;
    default:
        if ((opcode & 0xE000) == 0xE000) {
            char message[80];

            sprintf(message, "Opcode Parameter Not Processed: %x\n", opcode);
        } else {
            char message[80];

            sprintf(message, "Invalid User Animation Opcode: %x\n", opcode);
        }
        break;
    }
    return result;
}

// CAnimCallbacks::_AnimIntfOpcodeProcess follows in the original file and is
// not part of this unit.
