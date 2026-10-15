// A fragment of Soldier_object.cpp (0x800ac310): CSoldierObject's world and
// local bounding-volume getters: volume pointers for types 3 and 0, embedded
// volumes for types 1 and 2, none for other types; UpdateLocalBoundingVolume
// (type 3's volume box gets the new corners; the other types have nothing to
// update), CommitUpdate (CAnimObject's) and AttemptUpdate (CAnimObject's,
// then the type 1 and 2 world volumes become the local ones transformed by
// the soldier's matrix at +9024), UpdateAI (the aim matrix at +16224 placed
// at the soldier's position plus its rows scaled by the offset at +16288,
// then the AI doodad at +12512 updated) and BeginUpdate (CAnimObject's, the
// doodad's pre-update, the current and other weapons updated, a crouch timer
// and a countdown timer advanced in seconds at 60 per second) and
// DrawMuzzleFlash (on a flashing frame, a copy of the draw context and a
// random spin about Y: the flash node's matrix is built from the hand bone,
// the hand matrix and the flash matrix, or, when forced, from the attached
// object's attachment point named by the CRC 0xe6e804d5; then the flash node
// is placed and drawn).
// CSoldierObject, ISceneNode, IVolume, CVolBox, CAnimObject,
// CAISoldierDoodad, CWeapon, CDrawContext, CMatrix and EVolumeType are named
// by the mangled symbols; the members and their names are inferred (the
// draw-context view follows draw_context.cpp, the skeleton, attachment and
// muzzle-flash views are inferred; the flash node's slots +48 Draw and +232
// SetTMLocalToWorld follow the node tables), CAnimObject declares
// placeholder virtuals up to GetPosition (+64, named by the node table), the
// CVector3 view copies through a double pair as in player_weapon.cpp, and the
// empty cases reproduce the original's compare tree. The rest of the file is
// not part of this unit.
class CMatrix;

union CVector3Pair {
    double pair[2];
    float v[4];
};

/* inferred: CVector3 copies move it as a doubleword pair */
class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    CVector3& operator+=(const CVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
};

class CVolBox : public IVolume {
public:
    void SetCorners(CVector3, CVector3);
};

/* inferred: the matrix rows */
class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix(const CMatrix& other) { *this = other; }
    CMatrix& operator=(const CMatrix&);
    void Multiply(const CMatrix&, const CMatrix&);
    void RotateY(float);
    static void InitClass();
    static bool s_ClassInit;

    CVector3 GetRight() const { return right; }
    CVector3 GetFront() const { return front; }
    CVector3 GetUp() const { return up; }
    void SetPos(CVector3);

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

class CDrawContext;

/* inferred: the muzzle-flash node, its Draw (+48) and SetTMLocalToWorld (+232) */
class MuzzleFlashView {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void Draw(CDrawContext&);
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class CWeapon {
public:
    void Update(float, bool);
    bool GetMuzzleFlashStatus();

    unsigned char unknown000[708];
    float m_flashCounter;
    unsigned char unknown2c8[4];
    MuzzleFlashView* m_muzzleFlash;
};

/* inferred: the draw context copied by value (the view of draw_context.cpp) */
class CDrawContext {
public:
    CMatrix m_matrix0;
    CMatrix m_matrix40;
    CMatrix m_matrix80;
    int m_fieldC0;
    const CMatrix& m_source0;
    const CMatrix& m_source1;
    const CMatrix& m_source2;
    const CMatrix& m_source3;
    int m_fieldD4;
    int m_fieldD8;
    CVector3 m_fieldE0;
    int m_fieldF0;
    int m_fieldF4;
    int m_fieldF8;
    int m_fieldFC;
    int m_field100;
    float m_field104;
    float m_field108;
    float m_field10C;
    float m_field110;
    int m_field114;
};

/* inferred: an attachment point of the attached mesh (80 bytes) */
struct AttachPointView {
    CMatrix m_matrix;
    unsigned int m_nameCrc;
    unsigned char unknown44[12];
};

struct AttachListView {
    unsigned char unknown00[40];
    AttachPointView* m_points;
    int m_count;
};

struct AttachHolderView {
    unsigned char unknown00[4];
    AttachListView* m_list;
};

/* inferred: the object the weapon is attached to (+17544) */
struct AttachedObjectView {
    unsigned char unknown00[48];
    AttachHolderView* m_holder;
    unsigned char unknown34[12];
    CMatrix m_matrix;
};

struct SkeletonNodeView {
    unsigned char unknown00[16];
    int m_bone;
    unsigned char unknown14[4];
};

struct SkeletonView {
    unsigned char unknown00[20];
    SkeletonNodeView* m_nodes;
};

float MathFunRandomReal(float, float);

class CAISoldierDoodad {
public:
    void Update(float);
    void PreUpdate();

    unsigned char unknown0000[3712];
};

/* inferred: storage of an embedded volume object (its class is not known) */
struct EmbeddedVolumeView {
    unsigned char data[32];
};

class ISceneNode {
public:
    enum EVolumeType {};
};

class CAnimObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void GetPosition(CVector3&) const;

    void CommitUpdate();
    void AttemptUpdate(float);
    void UpdateAI(float);
    void BeginUpdate(float);
    void DrawMuzzleFlash(CDrawContext&, CWeapon*, bool);
};

class CSoldierObject : public CAnimObject {
public:
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    void CommitUpdate();
    void AttemptUpdate(float);
    void UpdateAI(float);
    void BeginUpdate(float);
    void DrawMuzzleFlash(CDrawContext&, CWeapon*, bool);

    unsigned char unknown0004[3296];
    SkeletonView* m_skeleton;
    unsigned char unknown0ce8[600];
    CMatrix m_bones[80];
    CMatrix m_matrix;
    IVolume* m_localVolume3;
    IVolume* m_localVolume0;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume0;
    unsigned char unknown2390[56];
    float m_impulseX;
    float m_impulseY;
    float m_impulseZ;
    unsigned char unknown23d4[3340];
    CAISoldierDoodad m_doodad;
    CMatrix m_aimMatrix;
    CVector3 m_aimOffset;
    unsigned char unknown3fb0[96];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
    unsigned char unknown4078[8];
    CMatrix m_handMatrices[8];
    unsigned char unknown4280[4];
    int m_handNode;
    unsigned char unknown4288[8];
    CMatrix m_flashMatrix;
    unsigned char unknown42d0[80];
    EmbeddedVolumeView m_localVolume1;
    EmbeddedVolumeView m_worldVolume1;
    EmbeddedVolumeView m_localVolume2;
    EmbeddedVolumeView m_worldVolume2;
    unsigned char unknown43a0[16];
    bool m_crouchTimerOn;
    float m_crouchTime;
    unsigned char unknown43b8[208];
    AttachedObjectView* m_attached;
    CWeapon* m_otherWeapon;
    float m_timer;
    int m_timerState;
};

IVolume* CSoldierObject::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_worldVolume3;
    case 0:
        return m_worldVolume0;
    case 1:
        return (IVolume*)&m_worldVolume1;
    case 2:
        return (IVolume*)&m_worldVolume2;
    case 4:
        return 0;
    }
    return 0;
}

IVolume* CSoldierObject::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_localVolume3;
    case 0:
        return m_localVolume0;
    case 1:
        return (IVolume*)&m_localVolume1;
    case 2:
        return (IVolume*)&m_localVolume2;
    case 4:
        return 0;
    }
    return 0;
}

void CSoldierObject::UpdateLocalBoundingVolume(ISceneNode::EVolumeType type, const CVector3& min, const CVector3& max) {
    switch (type) {
    case 3:
        {
            CVolBox* box = (CVolBox*)m_localVolume3;
            box->SetCorners(min, max);
        }
        break;
    case 0:
        break;
    case 1:
        break;
    case 4:
        break;
    }
}

void CSoldierObject::CommitUpdate() {
    CAnimObject::CommitUpdate();
}

void CSoldierObject::AttemptUpdate(float dt) {
    CAnimObject::AttemptUpdate(dt);
    GetWorldBoundingVolume((ISceneNode::EVolumeType)1)->TransformedCopy(*GetLocalBoundingVolume((ISceneNode::EVolumeType)1), m_matrix);
    GetWorldBoundingVolume((ISceneNode::EVolumeType)2)->TransformedCopy(*GetLocalBoundingVolume((ISceneNode::EVolumeType)2), m_matrix);
}

void CSoldierObject::UpdateAI(float dt) {
    CVector3 right = m_aimMatrix.GetRight();
    CVector3 front = m_aimMatrix.GetFront();
    CVector3 up = m_aimMatrix.GetUp();
    CVector3 pos;
    GetPosition(pos);
    right *= m_aimOffset.x;
    front *= m_aimOffset.y;
    up *= m_aimOffset.z;
    pos += right;
    pos += front;
    pos += up;
    m_aimMatrix.SetPos(pos);
    m_doodad.Update(dt);
}

void CSoldierObject::BeginUpdate(float dt) {
    float seconds = dt / 60.0f;
    CAnimObject::BeginUpdate(dt);
    m_doodad.PreUpdate();
    if (m_weapons[m_currentWeapon]) {
        m_weapons[m_currentWeapon]->Update(dt, false);
        m_otherWeapon->Update(dt, false);
    }
    if (m_crouchTimerOn) {
        m_impulseX = m_impulseY = m_impulseZ = 0.0f;
        m_crouchTime += seconds;
    }
    if (m_timer > 0.0f) {
        m_timer -= seconds;
    } else if (m_timer < 0.0f) {
        m_timer = 0.0f;
        m_timerState = 0;
    }
}

void CSoldierObject::DrawMuzzleFlash(CDrawContext& context, CWeapon* weapon, bool force) {
    CDrawContext flashContext(context);
    MuzzleFlashView* flash = weapon->m_muzzleFlash;
    if (!flash)
        return;
    if (!weapon->GetMuzzleFlashStatus() && !force)
        return;
    float angle = MathFunRandomReal(0.0f, 6.28f);
    if (((int)weapon->m_flashCounter & 3) && !force)
        return;
    CMatrix matrix;
    if (!force) {
        CMatrix bone = m_bones[m_skeleton->m_nodes[m_handNode].m_bone];
        matrix.Multiply(bone, m_matrix);
        CMatrix hand = m_handMatrices[m_handNode];
        matrix.Multiply(hand, matrix);
        CMatrix rotation = m_flashMatrix;
        rotation.RotateY(angle);
        matrix.Multiply(rotation, matrix);
    } else {
        AttachListView* list = m_attached->m_holder->m_list;
        int count = list->m_count;
        AttachPointView* point = list->m_points;
        matrix = m_attached->m_matrix;
        for (int i = 0; i < count; i++, point++) {
            if (point->m_nameCrc == 0xe6e804d5) {
                matrix.Multiply(point->m_matrix, matrix);
                break;
            }
        }
        matrix.RotateY(angle);
    }
    flash->SetTMLocalToWorld(matrix);
    flash->Draw(flashContext);
}
