// A fragment of animmot.cpp (0x800658c4): AnimMotCalcAnimPos and
// AnimMotCalcAnimRot blend the position and the rotation of the weight frames
// (position: frames flagged 4 or 8, bits 2/3 of the frame's byte at +8, for
// x/y and z; rotation: flagged 2). When a frame's channel state is the given
// state, or either is 0xffff, the result is marked changed; frames are only
// used when forced or so marked. A channel flagged 1 (position) or 8
// (rotation) is restarted (position cleared / base angle from the object and
// rotation cleared, the flag swapped for 2 / 16); one flagged 4 / 32 loses its
// three flags. A frame followed by a frame of the same channel is
// interpolated with it (or towards 0 when the channel is flagged 2 / 16) by the
// channel's factor, and the pair consumed. Position: the change of the
// channel's position is scaled by the channel's scale, its x/y turned by the
// channel's angle, and blended into the result by the frame's share of the
// running weight. Rotation: the change of the channel's rotation, scaled by
// its 8.24 fixed-point factor, is blended the same way. The result is stored
// when changed, and whether it changed is returned. The functions and
// AnimMotWeightFrame_t, AnimChannel_t (136 bytes), CVector3, CVector2 and
// OrientLerpAngle are named by the symbols; the members, the frame list and
// object views and the parameter meanings are inferred. The rest of the file
// is in other units.
/* inferred: CVector3 is copied as two doubles (16 bytes, 8-aligned) */
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { d = other.d; }
    void operator-=(const CVector3& o) {
        d.v[0] -= o.d.v[0];
        d.v[1] -= o.d.v[1];
        d.v[2] -= o.d.v[2];
    }
    void operator+=(const CVector3& o) {
        d.v[0] += o.d.v[0];
        d.v[1] += o.d.v[1];
        d.v[2] += o.d.v[2];
    }
    // each component of a is read just before its own result is stored
    void Lerp(const CVector3& a, const CVector3& b, float t) {
        float x = a.d.v[0];
        d.v[0] = t * (b.d.v[0] - x) + x;
        float y = a.d.v[1];
        d.v[1] = t * (b.d.v[1] - y) + y;
        float z = a.d.v[2];
        d.v[2] = t * (b.d.v[2] - z) + z;
    }
    void operator*=(float s) {
        d.v[0] *= s;
        d.v[1] *= s;
        d.v[2] *= s;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CVector2 {
public:
    CVector2() {}
    void Rotate(const CVector2&, long);

    float x;
    float y;
};

/* inferred: the object's angle at +8 */
struct AnimMotObjectView {
    unsigned char unknown00[8];
    int m_angle;
};

/* inferred: a 136-byte channel */
struct AnimChannel_t {
    unsigned char unknown00[14];
    unsigned short m_state;
    long m_angle;
    int m_rot;
    int m_rotScale;
    unsigned char unknown1c[4];
    CVector3 m_pos;
    float m_scale;
    unsigned char unknown34[56];
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
    CVector3 m_pos;
    unsigned char unknown20[48];
};

struct AnimMotFrameListView {
    unsigned short m_count;
    unsigned short m_max;
    unsigned char unknown04[4];
    AnimMotWeightFrame_t frames[24];
};

extern "C" bool AnimMotCalcAnimPos(void* object, AnimChannel_t* channels, AnimMotFrameListView* list, CVector3* out,
                                   unsigned short state, bool force) {
    CVector3 delta;
    CVector3 pos;
    CVector3 result;
    AnimChannel_t* channel;
    int i;
    AnimMotWeightFrame_t* frame;
    bool changed;
    result.d.v[2] = 0.0f;
    result.d.v[1] = 0.0f;
    result.d.v[0] = 0.0f;
    float total = 0.0f;
    changed = false;
    for (i = 0; i < list->m_count; i++) {
        frame = (AnimMotWeightFrame_t*)((char*)list + 8 + i * 80);
        if (!(frame->m_flags & 4) && !(frame->m_flags & 8))
            continue;
        bool use = force;
        int index = frame->m_channel;
        channel = &channels[index];
        if (channels[index].m_state == state || channel->m_state == 0xffff || state == 0xffff) {
            changed = true;
            use = true;
        }
        if (use != true)
            continue;
        if (channel->m_flags & 1) {
            channel->m_pos.d.v[2] = 0.0f;
            channel->m_pos.d.v[1] = 0.0f;
            channel->m_pos.d.v[0] = 0.0f;
            channel->m_flags &= ~1;
            channel->m_flags |= 2;
        }
        if (channel->m_flags & 4)
            channel->m_flags &= ~7;
        int next = i + 1;
        if (next < list->m_count && list->frames[next].m_channel == index) {
            AnimMotWeightFrame_t* other = &list->frames[next];
            if (!(channel->m_flags & 2)) {
                pos.Lerp(other->m_pos, frame->m_pos, channel->m_lerp);
            } else {
                pos.d.v[0] = channel->m_lerp * frame->m_pos.d.v[0];
                pos.d.v[1] = channel->m_lerp * frame->m_pos.d.v[1];
                pos.d.v[2] = channel->m_lerp * frame->m_pos.d.v[2];
            }
            i++;
        } else {
            pos = frame->m_pos;
        }
        if (!(frame->m_flags & 4)) {
            pos.d.v[0] = 0.0f;
            pos.d.v[1] = 0.0f;
        }
        if (!(frame->m_flags & 8))
            pos.d.v[2] = 0.0f;
        delta = pos;
        delta -= channel->m_pos;
        channel->m_pos += delta;
        delta *= channel->m_scale;
        CVector2 flat;
        flat.x = delta.d.v[0];
        flat.y = delta.d.v[1];
        flat.Rotate(flat, channel->m_angle);
        total += frame->m_weight;
        float t = frame->m_weight / total;
        delta.d.v[0] = flat.x;
        delta.d.v[1] = flat.y;
        result.Lerp(result, delta, t);
    }
    if (changed)
        *out = result;
    return changed;
}

extern "C" void OrientLerpAngle(int*, int, int, float);

/* 8.24 fixed-point product. CW defers functions that call inline functions
   to the end of the object, so both functions here use one to stay in order. */
static inline int FixedMul(int a, int b) {
    return (int)(((long long)a * b) >> 24);
}

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
            channel->m_angle = object->m_angle;
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
        OrientLerpAngle(&angle, FixedMul(delta, channel->m_rotScale), angle, frame->m_weight / total);
    }
    if (changed)
        *out = angle;
    return changed;
}
