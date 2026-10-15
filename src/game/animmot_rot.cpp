// A fragment of animmot.cpp (0x80065bd4): AnimMotCalcAnimRot blends the
// rotation of the weight frames flagged 2 (bit 1 of the frame's byte at +8):
// when a frame's channel state is the given state, or either is 0xffff, the
// result is marked changed; frames are only used when forced or so marked. A channel flagged 8 is
// restarted (its rotation time from the object, its rotation cleared, 8
// swapped for 16), one flagged 32 loses its 8/16/32 flags; a frame followed by
// a frame of the same channel is interpolated with it (or towards 0 when the
// channel is flagged 16) by the channel's factor, and the pair consumed; the
// change of the channel's rotation, scaled by its 8.24 fixed-point factor, is
// blended into the result by the frame's share of the running weight. The
// result is stored when changed, and whether it changed is returned. The
// function and AnimMotWeightFrame_t, AnimChannel_t (136 bytes) and
// OrientLerpAngle are named by the symbols; the members, the frame list and
// object views and the parameter meanings are inferred. The rest of the file
// is in other units.
/* inferred: the object's current time at +8 */
struct AnimMotObjectView {
    unsigned char unknown00[8];
    int m_time;
};

/* inferred: a 136-byte channel */
struct AnimChannel_t {
    unsigned char unknown00[14];
    unsigned short m_state;
    int m_rotTime;
    int m_rot;
    int m_rotScale;
    unsigned char unknown1c[80];
    float m_lerp;
    unsigned char unknown70[6];
    unsigned char m_flags;
    unsigned char unknown77[17];
};

struct AnimMotWeightFrame_t {
    float m_weight;
    float m_weight4;
    unsigned char m_flags;
    unsigned char m_channel;
    unsigned char unknown0a[2];
    int m_rot;
    unsigned char unknown10[64];
};

struct AnimMotFrameListView {
    unsigned short m_count;
    unsigned short m_max;
    unsigned char unknown04[4];
    AnimMotWeightFrame_t frames[24];
};

extern "C" void OrientLerpAngle(int*, int, int, float);

extern "C" bool AnimMotCalcAnimRot(AnimMotObjectView* object, AnimChannel_t* channels, AnimMotFrameListView* list,
                                   int* out, unsigned short state, bool force) {
    int rot;
    int angle = 0;
    float total = 0.0f;
    AnimChannel_t* channel;
    int i;
    AnimMotWeightFrame_t* frame;
    bool changed;
    changed = false;
    for (i = 0; i < list->m_count; i++) {
        frame = (AnimMotWeightFrame_t*)((char*)list + 8 + i * 80);
        if (!(frame->m_flags & 2))
            continue;
        int index = frame->m_channel;
        channel = &channels[index];
        bool use = force;
        if (channels[index].m_state == state || channel->m_state == 0xffff || state == 0xffff) {
            changed = true;
            use = true;
        }
        if (use != true)
            continue;
        if (channel->m_flags & 8) {
            channel->m_rotTime = object->m_time;
            channel->m_rot = 0;
            channel->m_flags &= ~8;
            channel->m_flags |= 16;
        }
        if (channel->m_flags & 32)
            channel->m_flags &= ~0x38;
        int next = i + 1;
        if (next < list->m_count && list->frames[next].m_channel == index) {
            if (!(channel->m_flags & 16))
                OrientLerpAngle(&rot, frame->m_rot, list->frames[next].m_rot, channel->m_lerp);
            else
                OrientLerpAngle(&rot, frame->m_rot, 0, channel->m_lerp);
            i++;
        } else {
            rot = frame->m_rot;
        }
        int delta = rot - channel->m_rot;
        channel->m_rot = channel->m_rot + delta;
        total += frame->m_weight;
        OrientLerpAngle(&angle, (int)(((long long)delta * channel->m_rotScale) >> 24), angle, frame->m_weight / total);
    }
    if (changed)
        *out = angle;
    return changed;
}
