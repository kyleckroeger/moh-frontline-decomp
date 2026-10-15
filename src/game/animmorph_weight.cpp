// A fragment of animmorph.cpp (0x800652a0): CAnimMorph::SetBlendTargetWeight
// stores a blend target's weight (the array at +3280) and flags the render
// object (+2624) for an update when the weight changed. The classes and
// functions are named by the mangled symbols; the members are inferred from
// offsets (the render object as in animated_bind.cpp). The rest of the file
// is not part of this unit.
struct DMRenderObject_T {
    unsigned char unknown00[20];
};

void DMRenderSetFlags(DMRenderObject_T*, unsigned long);

class CAnimMorph {
public:
    void SetBlendTargetWeight(unsigned long, float);

    unsigned char unknown0000[2624];
    DMRenderObject_T m_render;
    unsigned char unknown0a54[636];
    float m_weights[1];
};

void CAnimMorph::SetBlendTargetWeight(unsigned long target, float weight) {
    float old = m_weights[target];
    m_weights[target] = weight;
    if (weight != old)
        DMRenderSetFlags(&m_render, 1);
}
