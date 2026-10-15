// A fragment of animplayer.cpp (0x801099c4): weak copies of header inlines
// emitted in this file: CAnimatedPlayer's empty UpdateAI and AsAnimatedPlayerObject
// (itself), then CommitUpdate: the velocity constrained (a virtual call), then
// the position at +0x2370 copied to +0x23b8 (through a local), the word at
// +0x23b0 to +0x23ec, and the vector and four-float record at +0x2418 and
// +0x2428 to +0x23f8 and +0x2408. CAnimatedPlayer is named by the mangled
// symbols; the members and their types are inferred from the offsets and
// copies (the vectors move as doubleword pairs, the four-float record float
// by float). The virtual functions are declared in the order of
// __vt__15CAnimatedPlayer's first slots (MarkForDestruction, the destructor,
// Destroy, BeginUpdate, UpdateAI, CommitAI, ConstrainVelocity); the first is
// defined elsewhere, so the table is not emitted here. The rest of the file
// is not part of this unit.

/* Inferred: four floats overlaid with two doubles. */
class CVector3 {
public:
    union {
        float v[4];
        double pair[2];
    } d;
} __attribute__((aligned(8)));

/* Inferred: a four-float record copied float by float. */
struct CQuat4 {
    float x;
    float y;
    float z;
    float w;
};

class CAnimatedPlayer {
public:
    virtual void MarkForDestruction(int);
    virtual ~CAnimatedPlayer();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    void CommitUpdate();
    const CAnimatedPlayer* AsAnimatedPlayerObject() const;
    CAnimatedPlayer* AsAnimatedPlayerObject();

    unsigned char unknown0004[0x236c];
    CVector3 m_pos2370;
    unsigned char unknown2380[0x30];
    int m_value23b0;
    unsigned char unknown23b4[4];
    CVector3 m_pos23b8;
    unsigned char unknown23c8[0x24];
    int m_value23ec;
    unsigned char unknown23f0[8];
    CVector3 m_pos23f8;
    CQuat4 m_quat2408;
    CVector3 m_pos2418;
    CQuat4 m_quat2428;
};

__declspec(weak) void CAnimatedPlayer::UpdateAI(float) {
}

__declspec(weak) const CAnimatedPlayer* CAnimatedPlayer::AsAnimatedPlayerObject() const {
    return this;
}

__declspec(weak) CAnimatedPlayer* CAnimatedPlayer::AsAnimatedPlayerObject() {
    return this;
}

void CAnimatedPlayer::CommitUpdate() {
    ConstrainVelocity();
    CVector3 pos = m_pos2370;
    m_pos23b8 = pos;
    m_value23ec = m_value23b0;
    m_pos23f8 = m_pos2418;
    m_quat2408 = m_quat2428;
}
