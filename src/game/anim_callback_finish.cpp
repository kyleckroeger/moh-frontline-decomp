// A fragment of anim_callback.cpp (0x8005e6d4): CAnimCallbacks::
// _TurnFindTurnAnimInfo, then CAnimCallbacks::animfinish_cb.
// _TurnFindTurnAnimInfo picks the turn animation of a state's list: the turn
// angle (24-bit units) is the given angle converted (0: the heading from the
// object's position to its target, relative to its yaw); the first entry
// whose turn info covers it (minimum to maximum, wrapping at 2^24) is chosen.
// Its index is returned with the remaining frames-of-turn (the angle from the
// entry's centre, wrapped into +-2^23), its duration divided by the speed and
// two more values (the first negated); without a match the angle and every
// entry's range are reported in degrees and 0 is returned. animfinish_cb, the
// finish callback AnimCallback_Init registers, passes the finished state to
// the animated object's FinishAnimationState and returns 0. The names come
// from the mangled symbols; the list, turn-info and object views and the
// result types are inferred. The strings are .rodata and the constants
// .sdata2 pool entries.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

long MathArcTan2(float, float);
void DebugMsg(const char*, ...);

/* inferred: an entry's turn info: centre, minimum and maximum angle (24-bit
   units) and three values */
struct AnimTurnInfoView {
    unsigned char unknown00[12];
    long centre;
    long minimum;
    long maximum;
    float duration;
    float value1c;
    float value20;
};

struct AnimListEntryView {
    int anim;
    unsigned short flags;
    AnimTurnInfoView* info;
};

struct AnimFileStateAnimList_t {
    unsigned short count;
    AnimListEntryView entries[1];
};

class CAnimObject {
public:
    unsigned char unknown0000[9072];
    CVector3 m_position;
    unsigned char unknown2380[32];
    CVector3 m_target;
    long m_yaw;
};

class CAnimated {
public:
    void FinishAnimationState(unsigned short); /* result type not known */
};

class CAnimCallbacks {
public:
    static int animfinish_cb(unsigned short, void*);
    static unsigned short _TurnFindTurnAnimInfo(AnimFileStateAnimList_t*, CAnimObject*, unsigned short, long*, float*, float*, float*, float*, float);
};

unsigned short CAnimCallbacks::_TurnFindTurnAnimInfo(AnimFileStateAnimList_t* list, CAnimObject* object, unsigned short state, long* frames, float* duration, float* speed, float* value1, float* value2, float angle) {
    *frames = 0;
    *duration = 1.0f;
    *speed = 1.0f;
    long minimum;
    long wrapped;
    AnimTurnInfoView* found = 0;
    unsigned int index = 0;
    long turn;
    long maximum;
    AnimTurnInfoView* info;
    unsigned int i;
    if (angle == 0.0) {
        CVector3 target = object->m_target;
        CVector3 position = object->m_position;
        target.d.v[0] -= position.d.v[0];
        target.d.v[1] -= position.d.v[1];
        target.d.v[2] -= position.d.v[2];
        turn = ((MathArcTan2(-target.d.v[0], target.d.v[1]) & 0xffffff) - (object->m_yaw & 0xffffff)) & 0xffffff;
    } else {
        turn = 2670176.75f * angle;
    }
    for (i = 0; i < list->count; i++) {
        info = list->entries[i].info;
        wrapped = turn;
        minimum = info->minimum;
        maximum = info->maximum;
        if (minimum > maximum)
            maximum += 0x1000000;
        if (turn < minimum)
            wrapped += 0x1000000;
        if (wrapped >= minimum && wrapped <= maximum) {
            found = info;
            index = i;
            break;
        }
    }
    if (!found) {
        DebugMsg("Can't find turn Animation for %f!!\n", 360.0f * turn / 16777216.0f);
        for (int i = 0; i < list->count; i++) {
            AnimTurnInfoView* info = list->entries[i].info;
            DebugMsg("Anim %d (%x) min %f max %f\n", i, info, 360.0f * info->minimum / 16777216.0f, 360.0f * info->maximum / 16777216.0f);
        }
    }
    if (found && index < list->count) {
        long centre = found->centre;
        if (centre < minimum)
            centre += 0x1000000;
        *frames = wrapped - centre;
        if (*frames > 0x800000)
            *frames -= 0x1000000;
        else if (*frames < -0x800000)
            *frames += 0x1000000;
        *value1 = -found->value1c;
        *value2 = found->value20;
        *duration = found->duration;
        *duration = *duration / *speed;
    } else {
        index = 0;
    }
    return index;
}

int CAnimCallbacks::animfinish_cb(unsigned short state, void* user) {
    ((CAnimated*)user)->FinishAnimationState(state);
    return 0;
}
