// A fragment of anim_obj.cpp (0x80094358): CAnimObject's CommitUpdate
// (constrains the velocity, saves the position and yaw, takes the next bone
// position and rotation, and lifts the object onto the ground DetectGround
// finds), AttemptUpdate (moves by the velocity and turns by the turn rate -
// the yaw is a 24-bit angle - unless frozen, else takes the fixed transform;
// blends the root bone's pose, fires script event 45 below z -250, and
// rebuilds the world volume and the extent nodes, widened by 1/64) and
// BeginUpdate (updates the light volumes, saves the position, yaw and root
// bone pose, updates the animation, then stops, grounds or applies gravity
// to the vertical velocity, clamped to +-1.5). The file is compiled with
// deferred inlining (see anim_obj_draw.cpp), so the functions are written in
// reverse image order. The classes and functions are named by the symbols;
// CAnimObject declares its virtual functions in the order of
// __vt__11CAnimObject (placeholders named by offset for the slots not used
// here), the members, flag names and the extent view are inferred, the
// layout follows anim_obj_ctor.cpp, and the CVector3 view copies through a
// double pair as in player_weapon.cpp. The rest of the file is not part of
// this unit.
extern "C" {
#include <math.h>
}

struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix(const CMatrix& other) { *this = other; }
    CMatrix& operator=(const CMatrix&);
    void SetPos(CVector3);
    void Translate(CVector3);
    void BuildRotZ(float);
    void Multiply(const CMatrix&, const CMatrix&);
    CVector3 GetPos() const { return position; }

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    void Slerp(CQuaternion, CQuaternion, float);
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
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
};

class CVolSphere {
public:
    void SetCenter(CVector3);
};

class CLightVolumeManager {
public:
    void Update(float);

    unsigned char unknown00[4];
};

class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float);
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void SetRotation(long);
};

class CAnimSkinned : public CAnimated {
public:
    unsigned char unknown04[44];
};

/* inferred: a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

class CAnimObject {
public:
    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void GetPosition(CVector3&) const;
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void vf0e4();
    virtual void vf0e8();
    virtual void vf0ec();
    virtual void vf0f0();
    virtual void vf0f4();
    virtual void vf0f8();
    virtual void vf0fc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void vf124();
    virtual void UpdateAnimation(float);

    float DetectGround();

    unsigned char unknown0004[8];
    ExtentNodeView* m_extentMinX;
    ExtentNodeView* m_extentMaxX;
    ExtentNodeView* m_extentMinY;
    ExtentNodeView* m_extentMaxY;
    unsigned char unknown001c[20];
    CAnimSkinned m_skinned;
    unsigned char unknown0060[3808];
    CMatrix m_matrices[80];
    CMatrix m_tm;
    IVolume* m_localVolume3;
    IVolume* m_localVolume;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume;
    BSObject* m_script;
    unsigned char unknown2394[4];
    CVolSphere* m_sphere;
    unsigned char unknown239c[20];
    long m_yaw;
    unsigned char unknown23b4[4];
    CVector3 m_lastPosition;
    CVector3 m_velocity;
    unsigned char unknown23d8[20];
    long m_lastYaw;
    float m_turn;
    unsigned char unknown23f4[4];
    CVector3 m_bonePosition;
    CQuaternion m_boneRotation;
    CVector3 m_nextBonePosition;
    CQuaternion m_nextBoneRotation;
    unsigned char unknown2438[8];
    int m_value2440;
    unsigned char unknown2444[12];
    bool m_flag2450;
    unsigned char unknown2451[3];
    bool m_onGround;
    bool m_falling;
    unsigned char unknown2456[4];
    bool m_frozen;
    bool m_useFixedTM;
    unsigned char unknown245c[4];
    CMatrix m_fixedTM;
    unsigned char unknown24a0[18];
    bool m_keepZ;
    unsigned char unknown24b3[2];
    bool m_stopped;
    unsigned char unknown24b6[2];
    CLightVolumeManager m_lightVolumes;
};

void CAnimObject::BeginUpdate(float dt) {
    m_lightVolumes.Update(dt);
    m_lastPosition = m_tm.GetPos();
    m_lastYaw = m_yaw;
    CMatrix bone(m_matrices[1]);
    m_boneRotation.SetFromMatrix(bone);
    m_bonePosition = bone.GetPos();
    UpdateAnimation(dt);
    if (m_frozen || m_stopped) {
        m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    } else if (m_onGround && m_keepZ) {
        m_velocity.z = 0.0f;
    } else if (m_keepZ) {
        m_velocity.z += -0.005455351f * dt;
        float vz = m_velocity.z;
        if ((float)fabs(vz) > 1.5f) {
            if (vz > 0.0f)
                m_velocity.z = 1.5f;
            else
                m_velocity.z = -1.5f;
        }
        m_falling = true;
    }
    m_flag2450 = false;
}

void CAnimObject::AttemptUpdate(float dt) {
    CVector3 move;
    move.x = dt * m_velocity.x;
    move.y = dt * m_velocity.y;
    move.z = dt * m_velocity.z;
    float turn = m_turn * dt;
    CMatrix bone(m_matrices[1]);
    CQuaternion rotation;
    rotation.SetFromMatrix(bone);
    CVector3 position = bone.position;
    float pz = m_bonePosition.z;
    float py = m_bonePosition.y;
    float px = m_bonePosition.x;
    m_nextBonePosition.x = px + dt * (position.x - px);
    m_nextBonePosition.y = py + dt * (position.y - py);
    m_nextBonePosition.z = pz + dt * (position.z - pz);
    m_nextBoneRotation.Slerp(rotation, m_boneRotation, dt);
    m_nextBoneRotation.GetMatrix(bone);
    bone.SetPos(m_nextBonePosition);
    if (m_tm.position.z < -250.0f)
        BSObjectTriggerEvent(m_script, 45, 0, 0, false);
    if (!m_frozen) {
        m_yaw = (m_lastYaw + ((long)(2670176.8f * turn) & 0xffffff)) & 0xffffff;
        m_tm.BuildRotZ(6.2831855f * (float)m_yaw / 16777216.0f);
        m_tm.SetPos(m_lastPosition);
        m_tm.Translate(move);
        m_skinned.SetRotation(m_yaw);
    } else if (m_useFixedTM) {
        m_tm = m_fixedTM;
    }
    if (m_localVolume) {
        bone.Multiply(bone, m_tm);
        m_sphere->SetCenter(CVector3(0.0f, 0.0f, 0.0f));
        m_worldVolume->TransformedCopy(*m_localVolume, bone);
        CVector3 min;
        CVector3 max;
        m_worldVolume->GetExtents(min, max);
        m_extentMinX->m_value = min.x - 0.015625f;
        m_extentMinY->m_value = min.y - 0.015625f;
        m_extentMaxX->m_value = 0.015625f + max.x;
        m_extentMaxY->m_value = 0.015625f + max.y;
    } else {
        CVector3 p;
        GetPosition(p);
        m_extentMinX->m_value = p.x - 0.015625f;
        m_extentMinY->m_value = p.y - 0.015625f;
        m_extentMaxX->m_value = 0.015625f + p.x;
        m_extentMaxY->m_value = 0.015625f + p.y;
    }
    m_onGround = false;
    m_value2440 = 0;
    m_flag2450 = false;
}

void CAnimObject::CommitUpdate() {
    ConstrainVelocity();
    m_lastPosition = m_tm.GetPos();
    m_lastYaw = m_yaw;
    m_bonePosition = m_nextBonePosition;
    m_boneRotation = m_nextBoneRotation;
    float ground = DetectGround();
    if (ground > 0.0f) {
        m_tm.Translate(CVector3(0.0f, 0.0f, ground));
        m_lastPosition = m_tm.GetPos();
    }
}
