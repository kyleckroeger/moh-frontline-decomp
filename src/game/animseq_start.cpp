// A fragment of animseq.cpp (0x80066388): AnimSeqStart starts a sequence on
// a frame list: the rate is the list's frame rate / 16 (1 when not above
// 0), the time starts at the rate and the life at 0, the frame count is
// unknown (0xffff), and the first two frames are parsed (the second is the
// first again, with a nonzero result, when the list ends). The structs are
// named by the mangled symbols and follow animseq.cpp; the members after
// +16 are named by offset, and the result's meaning is inferred. The rest
// of the file is in other units. _AnimSeqParseToEnd (file-local) starts
// the same way (parsing with the flag set), then reads opcodes until the
// target or an end opcode, counting frames (their life) and skipping the
// extra word of the 0xe000 family; user opcodes go to the skip callback.
struct AnimSeqFrameList_t {
    unsigned char framerate;
    unsigned char field01[7];
    unsigned short opcodes[1];
};

struct AnimSeqAnimInfo_t {
    AnimSeqFrameList_t* list;
    float rate;
    float life;
    float time;
    unsigned short frame10;
    unsigned short frame12;
    float value14;
    unsigned short position;
    unsigned short frames;
    unsigned short value1C;
    unsigned char flag1E;
    unsigned char flag1F;
    unsigned char field20[16];
};

// File-local in the original; declared without static here because this
// fragment does not define it.
unsigned short _ParseForFrame(AnimSeqAnimInfo_t*, float, unsigned char);
void _AnimDoUserOpcodeSkipCallback(unsigned short);

extern AnimSeqAnimInfo_t* _AnimSeq_CurrentInfo;

extern "C" int AnimSeqStart(AnimSeqAnimInfo_t* info, AnimSeqFrameList_t* list) {
    int ended = 0;
    info->list = list;
    info->rate = list->framerate;
    info->rate *= 0.0625f;
    if (info->rate <= 0.0f)
        info->rate = 1.0f;
    info->frames = 0xffff;
    info->value1C = 0;
    info->life = 0.0f;
    info->time = info->rate;
    info->position = 0;
    info->flag1F = 0;
    info->flag1E = 0;
    info->frame10 = _ParseForFrame(info, 0.0f, 0);
    info->frame12 = _ParseForFrame(info, 0.0f, 0);
    info->value14 = 0.0f;
    if (info->frame12 == 0xffff) {
        ended = 1;
        info->frame12 = info->frame10;
    }
    return ended;
}

static void _AnimSeqParseToEnd(AnimSeqAnimInfo_t* info, AnimSeqFrameList_t* list, unsigned short target) {
    bool done = false;
    info->list = list;
    info->rate = list->framerate;
    info->rate *= 0.0625f;
    if (info->rate <= 0.0f)
        info->rate = 1.0f;
    info->frames = 0xffff;
    info->value1C = 0;
    info->life = 0.0f;
    info->time = info->rate;
    info->position = 0;
    info->flag1F = 0;
    info->flag1E = 0;
    info->frame10 = _ParseForFrame(info, 0.0f, 1);
    info->frame12 = _ParseForFrame(info, 0.0f, 1);
    info->value14 = 0.0f;
    if (info->frame12 == 0xffff)
        info->frame12 = info->frame10;
    info->life += 2.0f * info->time;
    while (!done) {
        unsigned short op = info->list->opcodes[info->position];
        info->position++;
        if ((op & 0xc000) == 0xc000) {
            if (op == target)
                done = true;
            if (!(op & 0x800)) {
                switch (op) {
                case 0xc000:
                    info->life -= info->time;
                case 0xc001:
                    if (info->frames == 0xffff)
                        info->frames = info->value1C;
                    done = true;
                    break;
                case 0xc002:
                case 0xc003:
                case 0xc004:
                case 0xc005:
                case 0xc006:
                    break;
                default:
                    if ((op & 0xe000) == 0xe000 && (op & 0x1000) == 0x1000)
                        info->position++;
                    break;
                }
            } else {
                _AnimSeq_CurrentInfo = info;
                _AnimDoUserOpcodeSkipCallback(op);
            }
        } else {
            info->life += info->time;
            info->value1C++;
        }
    }
}
