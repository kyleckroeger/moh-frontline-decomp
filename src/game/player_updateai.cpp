// A fragment of player.cpp (0x800a2464): CPlayerObject::AttemptUpdate and
// UpdateAI. AttemptUpdate places the multiplayer mesh (+1992) like
// CommitUpdate, stores the frame time (+1880), rebuilds the transform from
// the yaw (+668) and position (+648) and moves it by the velocity (+600)
// times the time (unless bit 0x80 or 0x02 of +918 is set), advances the
// crouch height (+664) by its speed (+676), clamped to 1.1 and a minimum
// (0.7 in multiplayer, else 0; either clamp clears bit 0x40 of +917), sets
// the local capsule's axis from the origin up to that height, transforms it
// into the world capsule and updates the sweep endpoints (+12..+24) from its
// extents widened by CONTACT_EPSILON, then clears the contact bits of +916
// and the values at +688 (unless bit 0x80 of +918) and +912. UpdateAI
// updates the player's AI doodad (+36). The classes and functions are named
// by the symbols; the members, flag names and views are inferred, including
// the CVector3 double-pair view's constructor from the union (a whole copy,
// used for the position passed to SetPos). The rest of the file is in other
// units.
/* inferred: the vector's doubleword-pair view */
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) { d.pair[0] = o.d.pair[0]; d.pair[1] = o.d.pair[1]; }
    /* inferred: a whole (builtin) copy from the union view */
    CVector3(const CVector3Data& o) : d(o) {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    CLine3& operator=(const CLine3& other) {
        if (&other != this) {
            m_start.d = other.m_start.d;
            m_end.d = other.m_end.d;
            m_dir.d = other.m_dir.d;
            m_value30 = other.m_value30;
            m_value34 = other.m_value34;
            m_flag38 = other.m_flag38;
            m_flag39 = other.m_flag39;
        }
        return *this;
    }
    void SetSE(CVector3 start, CVector3 end) {
        m_start.d = start.d;
        m_end.d = end.d;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

class CMatrix {
public:
    CMatrix(const CMatrix& other) { *this = other; }
    CMatrix& operator=(const CMatrix&);
    void BuildRotZ(float);
    void SetPos(CVector3);
    void Translate(CVector3);
    const CVector3& GetPos() const { return m_pos; }

    CVector3 m_right;
    CVector3 m_front;
    CVector3 m_up;
    CVector3 m_pos;
} __attribute__((aligned(16)));

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
};

class CVolCapsule : public IVolume {
public:
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_axis;
};

/* inferred: the animated mesh the multiplayer player drives */
class CAnimMeshView {
public:
    virtual void vf08();
};

class CAnimMesh {
public:
    void* m_vtable;
};

/* inferred: a sweep endpoint (value at +8) */
struct SweepEndpoint {
    unsigned char unknown00[8];
    float m_value;
};

class CAIPlayerDoodad {
public:
    void Update(float);
};

extern bool g_bInMultiplayerMode;
extern float g_MPAnimMeshZOffset;
extern const float CONTACT_EPSILON;

class CPlayerMeshView;

class CPlayerObject {
public:
    void AttemptUpdate(float);
    void UpdateAI(float);

    unsigned char unknown000[12];
    SweepEndpoint* m_minX;
    SweepEndpoint* m_maxX;
    SweepEndpoint* m_minY;
    SweepEndpoint* m_maxY;
    unsigned char unknown01c[8];
    CAIPlayerDoodad m_doodad;
    unsigned char unknown025[91];
    CMatrix m_tm;
    unsigned char unknown0c0[240];
    CVolCapsule m_localCapsule;
    CVolCapsule m_worldCapsule;
    unsigned char unknown250[8];
    CVector3 m_velocity;
    unsigned char unknown268[32];
    CVector3 m_position;
    float m_crouchHeight;
    float m_rotation;
    unsigned char unknown2a0[4];
    float m_heightSpeed;
    unsigned char unknown2a8[8];
    int m_value2b0;
    unsigned char unknown2b4[220];
    float m_value390;
    unsigned char m_flag394a : 1;
    unsigned char m_flag394b : 1;
    unsigned char m_flag394c : 1;
    unsigned char m_flags394 : 5;
    unsigned char m_flag395a : 1;
    unsigned char m_flag395b : 1;
    unsigned char m_flags395 : 6;
    unsigned char m_flag396a : 1;
    unsigned char m_flags396 : 5;
    unsigned char m_flag396g : 1;
    unsigned char m_flag396h : 1;
    unsigned char m_flag397a : 1;
    unsigned char m_flag397b : 1;
    unsigned char m_flags397 : 6;
    unsigned char unknown398[960];
    float m_delta;
    unsigned char unknown75c[108];
    class CMeshNode* m_mesh;
};

class CMeshNode {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual void vf9c();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual void vfbc();
    virtual void vfc0();
    virtual void vfc4();
    virtual void vfc8();
    virtual void vfcc();
    virtual void vfd0();
    virtual void vfd4();
    virtual void vfd8();
    virtual void vfdc();
    virtual void vfe0();
    virtual void vfe4();
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void vfec();
    virtual void vff0();
    virtual void vff4();
    virtual void vff8();
    virtual void vffc();
    virtual void vf100();
    virtual void vf104();
    virtual void SetPosition(CVector3);
};

void CPlayerObject::AttemptUpdate(float delta) {
    if (g_bInMultiplayerMode) {
        if (m_flag397b) {
            CVector3 position = m_tm.GetPos();
            position.d.v[2] += g_MPAnimMeshZOffset;
            m_mesh->SetPosition(position);
        } else {
            CMatrix tm(m_tm);
            tm.Translate(CVector3(0.0f, 0.0f, g_MPAnimMeshZOffset));
            m_mesh->SetTMLocalToWorld(tm);
        }
    }
    m_delta = delta;
    CVector3 move(delta * m_velocity.d.v[0], delta * m_velocity.d.v[1], delta * m_velocity.d.v[2]);
    if (!m_flag396a && !m_flag396g) {
        m_tm.BuildRotZ(m_rotation);
        m_tm.SetPos(m_position.d);
        m_tm.Translate(move);
    }
    float height = m_heightSpeed * delta + m_crouchHeight;
    float low = g_bInMultiplayerMode ? 0.7f : 0.0f;
    if (height > 1.1f) {
        m_flag395b = 0;
        height = 1.1f;
    } else if (height < low) {
        m_flag395b = 0;
        height = low;
    }
    CVector3 start;
    start.d.v[0] = start.d.v[1] = start.d.v[2] = 0.0f;
    CVector3 end(0.0f, 0.0f, height);
    m_localCapsule.m_axis = CLine3(start, end);
    m_worldCapsule.TransformedCopy(m_localCapsule, m_tm);
    CVector3 low3;
    CVector3 high3;
    m_worldCapsule.GetExtents(low3, high3);
    m_minX->m_value = low3.d.v[0] - CONTACT_EPSILON;
    m_minY->m_value = low3.d.v[1] - CONTACT_EPSILON;
    m_maxX->m_value = CONTACT_EPSILON + high3.d.v[0];
    m_maxY->m_value = CONTACT_EPSILON + high3.d.v[1];
    m_flag394b = 0;
    m_flag394c = 0;
    m_flag394a = 0;
    if (!m_flag396a)
        m_value2b0 = 0;
    m_value390 = 0.0f;
}

void CPlayerObject::UpdateAI(float delta) {
    m_doodad.Update(delta);
}
