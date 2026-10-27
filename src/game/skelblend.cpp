// A fragment of skelblend.cpp (0x8006a39c): BlendSkelFrames unpacks the
// first weighted frame of the list into the output frame, then blends every
// further frame into it: each is unpacked into a local frame, the position is
// interpolated by the frame's weight, every bone's three short values by the
// weight in 1/4096 steps (scaled by 4095 and truncated), and the orientation
// angle through OrientLerpAngle. BlendSkelFrames, _unpack_skel_frame (local
// to the file, defined earlier, outside this fragment), OrientLerpAngle,
// CharFrame_t, AnimMotFrameList_t and AnimMotWeightFrame_t are named by the
// symbols; the frame layout (bone count +0, position +8, angle +40, bone
// values +56 pointing at storage of 80 triples from +64; 16-byte aligned,
// from the stack frame), the list layout (count +0, 80-byte weighted frames
// from +8, weight at +4 of each) and the BlendValues helper are inferred.
struct CharFrame_t {
    int m_count;
    int unknown04;
    float m_position[3];
    unsigned char unknown14[20];
    int m_angle;
    unsigned char unknown2c[12];
    short* m_values;
    int unknown3c;
    short m_storage[80 * 3];
} __attribute__((aligned(16)));

struct AnimMotWeightFrame_t {
    int unknown00;
    float m_weight;
    unsigned char unknown08[72];
};

/* inferred: the count, then the weighted frames */
struct AnimMotFrameList_t {
    unsigned short m_count;
    unsigned char unknown02[6];
    AnimMotWeightFrame_t m_frames[1];
};

extern "C" void OrientLerpAngle(int*, int, int, float);
/* local to the original file (defined before BlendSkelFrames, outside this
   fragment); declared extern here so the fragment links to it */
void _unpack_skel_frame(CharFrame_t*, AnimMotWeightFrame_t*);

/* inferred: moves each bone's values towards the frame's by the weight */
static inline void BlendValues(CharFrame_t* out, CharFrame_t* frame, float weight) {
    int count;
    int fixed;
    short* from;
    short* to;
    fixed = (int)(4095.0f * weight);
    count = out->m_count;
    from = frame->m_values;
    to = out->m_values;
    for (int j = count; j != 0; j--) {
        to[0] = to[0] + ((short)(from[0] - to[0]) * fixed >> 12);
        to[1] = to[1] + ((short)(from[1] - to[1]) * fixed >> 12);
        to[2] = to[2] + ((short)(from[2] - to[2]) * fixed >> 12);
        to += 3;
        from += 3;
    }
}

extern "C" void BlendSkelFrames(CharFrame_t* out, AnimMotFrameList_t* list) {
    CharFrame_t frame;
    unsigned int i;
    unsigned int count;
    float weight;
    float z;
    float y;
    float x;

    count = list->m_count;
    if (count == 0)
        return;
    frame.m_values = frame.m_storage;
    _unpack_skel_frame(out, &list->m_frames[0]);
    for (i = 1; i < count; i++) {
        weight = list->m_frames[i].m_weight;
        _unpack_skel_frame(&frame, &list->m_frames[i]);
        x = out->m_position[0];
        y = out->m_position[1];
        z = out->m_position[2];
        out->m_position[0] = weight * (frame.m_position[0] - x) + x;
        out->m_position[1] = weight * (frame.m_position[1] - y) + y;
        out->m_position[2] = weight * (frame.m_position[2] - z) + z;
        BlendValues(out, &frame, weight);
        OrientLerpAngle(&out->m_angle, frame.m_angle, out->m_angle, weight);
    }
}
