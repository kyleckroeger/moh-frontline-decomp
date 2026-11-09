// A fragment of static_obj.cpp (0x800b013c): CStaticObject's Draw (when
// the object is shown, points the draw context at its transform, sets the
// light volume when the mesh is lit and draws the mesh), CommitUpdate (tells
// the other matrix listener, commits the path, script or animation motion
// that is running and gives the attached object the transform moved by its
// offset), CommitPathMotionUpdate (saves the transform's position as the path
// motion's position, copies a speed value and recomputes the path destination
// and speed), CommitAnimMotionUpdate (while animation motion is active, saves
// the position and sets the rotation at +336 from the transform),
// CommitScriptMotionUpdate (saves the position, then for each active script
// motion - flag bits at +480 - steps its time down by its step, never below
// 0 (a .sdata2 pool entry), the rotation one also taking its target rotation
// and the value one advancing its value by step times rate) and
// AttemptUpdate (attempts the running motion; script motion only when the
// hold flag at +481 is clear). CStaticObject, CMatrix, CQuaternion,
// CStaticMesh, CLightVolumeManager and CDrawContext are named by the mangled
// symbols; CStaticObject declares its virtual functions in the order of
// __vt__13CStaticObject (placeholders named by offset for the slots not used
// here), its members, the flag names and the mesh, attached-object,
// listener and draw-context views are inferred, and the CVector3 view and
// by-value position getter are as in static_obj_setpos.cpp. The rest of the
// file is not part of this unit.
#include <math.h>

struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CVector3 {
public:
    union {
        double pair[2];
        struct {
            float x;
            float y;
            float z;
            float w;
        } c;
    };
} __attribute__((aligned(8)));

inline void Normalize(CVector3& v) {
    float length = sqrtf(v.c.x * v.c.x + v.c.y * v.c.y + v.c.z * v.c.z);
    if (length != 0.0f) {
        float scale = 1.0f / length;
        v.c.x *= scale;
        v.c.y *= scale;
        v.c.z *= scale;
    }
}

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    void PreTranslate(CVector3);
    void SetPos(CVector3);
    void FastInverse(const CMatrix&);
    void TransformPoint(CVector3&, CVector3) const;
    void BuildScale(float);
    void Multiply(const CMatrix&, const CMatrix&);
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

class BPDLightVolume;

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();

    unsigned char unknown00[4];
};

class CStaticMesh {
public:
    static void SetLightVolume(BPDLightVolume*);
};

/* inferred: the draw context's transform pointer at +192 */
class CDrawContext {
public:
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
};

/* inferred: the object's mesh (+48), drawn through its slot +12 */
class MeshView {
public:
    virtual void unknown08();
    virtual void Draw(CDrawContext&, int);
    virtual void unknown10();
    virtual bool IsLit();
};

/* inferred: the object at +588, given the object's transform through its slot +232 */
class AttachedView {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
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
    virtual void SetTransform(CMatrix&);
};

/* inferred: the other matrix listener, told through its slot +16 */
class MatrixListenerView {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void Update(void*, const CMatrix&);
};

/* inferred: the other node of a collision (slots +64, +96, +156, +188) */
class OtherNodeView {
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
    virtual void GetPosition(CVector3&) const;
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual BSObject* GetScriptObject() const;
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
    virtual OtherNodeView* AsPlayerObject();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual void* AsBullet();
};

/* inferred: a contact (point at +16) */
struct ContactView {
    unsigned char unknown00[16];
    CVector3 m_point;
};

class CCollision {
public:
    OtherNodeView* GetOther(const void* node) const {
        OtherNodeView* other = m_nodeA;
        if (other == node)
            other = m_nodeB;
        return other;
    }

    OtherNodeView* m_nodeA;
    OtherNodeView* m_nodeB;
    unsigned char unknown08[16];
    ContactView* m_contacts;
};

/* inferred: the collision damage event block (script event 192) */
struct CollisionDamageEvent {
    void* m_owner;
    int m_value04;
    int m_value08;
    int m_value0c;
    int m_value10;
};

struct CheatsView {
    bool m_invincible;
    unsigned char unknown01[19];
};

extern CheatsView g_cheats;

class CStaticObject {
public:
    virtual void MarkForDestruction(int);
    virtual ~CStaticObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual void GetLocalBoundingVolume(int) const;
    virtual void GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void vf040();
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
    virtual void vf128();
    virtual void vf12c();
    virtual void vf130();
    virtual void vf134();
    virtual void vf138();
    virtual void vf13c();
    virtual void vf140();
    virtual void vf144();
    virtual void vf148();
    virtual void AttemptScriptMotionUpdate(float);
    virtual void AttemptAnimMotionUpdate(float);
    virtual void AttemptPathMotionUpdate(float);
    virtual void CommitScriptMotionUpdate();
    virtual void CommitAnimMotionUpdate();
    virtual void CommitPathMotionUpdate();

    bool IsFollowingPath() const { return m_pathState != 0 && m_pathState != 1; }
    void UpdatePathMotionDestAndSpeed();
    void AttemptCollisionDamage(const CCollision& collision) {
        if (!m_damageable)
            return;
        OtherNodeView* other;
        OtherNodeView* node;
        float zero = 0.0f;
        if (m_value0e0 == zero && m_speed216 == zero)
            return;
        other = node = collision.GetOther(this);
        CollisionDamageEvent event;
        event.m_owner = m_ownerRef;
        event.m_value04 = 1;
        event.m_value10 = 9;
        event.m_value08 = 1;
        event.m_value0c = 5;
        if (g_cheats.m_invincible && node->AsPlayerObject())
            event.m_value04 = 0;
        if (other->GetScriptObject())
            BSObjectTriggerEvent(other->GetScriptObject(), 192, &event, 0, true);
    }

    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown004[44];
    MeshView* m_mesh;
    unsigned char unknown034[12];
    CMatrix m_tm;
    unsigned char unknown080[88];
    float m_speed216;
    unsigned char unknown0dc[4];
    float m_value0e0;
    unsigned char unknown0e4[32];
    int m_pathState;
    unsigned char unknown108[4];
    void* m_listenerData;
    MatrixListenerView* m_matrixListener;
    MatrixListenerView* m_listener;
    void* m_ownerRef;
    unsigned char unknown11c[4];
    CVector3 m_animPosition;
    unsigned char unknown130[32];
    CQuaternion m_animRotation;
    float m_velocityX;
    float m_velocityY;
    float m_velocityZ;
    unsigned char unknown16c[4];
    CVector3 m_pathPosition;
    CQuaternion m_scriptRotation;
    unsigned char unknown190[16];
    CQuaternion m_scriptTargetRotation;
    float m_value1b0;
    float m_value1b4;
    float m_time1b8;
    float m_time1bc;
    float m_time1c0;
    unsigned char unknown1c4[12];
    float m_step1d0;
    float m_step1d4;
    float m_step1d8;
    unsigned char unknown1dc[4];
    unsigned char m_flag80 : 1;
    unsigned char m_scriptMove : 1;
    unsigned char m_scriptRotate : 1;
    unsigned char m_scriptValue : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_animMove : 1;
    unsigned char m_flag02 : 1;
    bool m_damageable : 1;
    unsigned char m_flags1e1a : 2;
    unsigned char m_scriptHold : 1;
    unsigned char m_flags1e1b : 5;
    unsigned char unknown1e2[22];
    float m_pathSpeed;
    unsigned char unknown1fc[24];
    int m_drawArg;
    unsigned char unknown218[12];
    int m_animMotion;
    unsigned char unknown228[4];
    int m_animMotionActive;
    unsigned char unknown230[8];
    CQuaternion m_rotation238;
    unsigned char unknown248[4];
    AttachedView* m_attached;
    CVector3 m_attachOffset;
    CLightVolumeManager m_lightVolumes;
};

void CStaticObject::OnCollision(const CCollision& collision) {
    AttemptCollisionDamage(collision);
    if (IsFollowingPath()) {
        m_step1d0 = 0.0f;
        m_step1d4 = 0.0f;
        m_step1d8 = 0.0f;
        m_scriptTargetRotation = m_scriptRotation;
        if (!collision.GetOther(this)->AsBullet()) {
            m_tm.SetPos(m_pathPosition);
            m_pathSpeed = 0.0f;
            m_speed216 = 0.0f;
        }
    } else if (m_scriptMove || m_scriptRotate || m_scriptValue) {
        OtherNodeView* other = collision.GetOther(this);
        bool stop = true;
        OtherNodeView* player = other->AsPlayerObject();
        if (player && collision.m_contacts && !m_scriptValue) {
            CMatrix tm;
            CVector3 local;
            CVector3 point = collision.m_contacts->m_point;
            tm.FastInverse(m_tm);
            tm.TransformPoint(local, point);
            m_scriptRotation.GetMatrix(tm);
            tm.SetPos(m_pathPosition);
            tm.TransformPoint(local, local);
            CVector3 position;
            player->GetPosition(position);
            CVector3 away;
            away.c.x = point.c.x - position.c.x;
            away.c.y = point.c.y - position.c.y;
            away.c.z = point.c.z - position.c.z;
            Normalize(away);
            CVector3 motion;
            motion.c.z = point.c.z;
            motion.c.z -= local.c.z;
            float px = point.c.x;
            motion.c.x = px;
            motion.c.y = point.c.y;
            motion.c.y -= local.c.y;
            motion.c.x -= local.c.x;
            Normalize(motion);
            if (away.c.x * motion.c.x + away.c.y * motion.c.y + away.c.z * motion.c.z > 0.0f)
                stop = false;
        }
        if (stop) {
            m_scriptHold = 1;
            m_step1d0 = 0.0f;
            m_step1d4 = 0.0f;
            m_step1d8 = 0.0f;
            m_scriptTargetRotation = m_scriptRotation;
            m_scriptRotation.GetMatrix(m_tm);
            m_tm.SetPos(m_pathPosition);
            CMatrix scale;
            scale.BuildScale(m_value1b4);
            m_tm.Multiply(scale, m_tm);
        }
    } else if (m_animMove) {
        m_step1d0 = 0.0f;
        m_step1d4 = 0.0f;
        m_step1d8 = 0.0f;
        m_scriptTargetRotation = m_scriptRotation;
        m_velocityZ = 0.0f;
        m_velocityY = 0.0f;
        m_velocityX = 0.0f;
        m_rotation238.SetFromMatrix(m_tm);
    }
}

void CStaticObject::Draw(CDrawContext& context) {
    if (m_flag80) {
        context.m_localToWorld = &m_tm;
        if (m_mesh->IsLit())
            CStaticMesh::SetLightVolume(m_lightVolumes.GetVolume());
        m_mesh->Draw(context, m_drawArg);
    }
}

void CStaticObject::CommitUpdate() {
    if (m_matrixListener != m_listener)
        m_matrixListener->Update(m_listenerData, m_tm);
    if (IsFollowingPath())
        CommitPathMotionUpdate();
    else if (m_scriptMove || m_scriptRotate || m_scriptValue)
        CommitScriptMotionUpdate();
    else if (m_animMove)
        CommitAnimMotionUpdate();
    if (m_attached) {
        CMatrix tm;
        GetTMLocalToWorld(tm);
        tm.PreTranslate(m_attachOffset);
        m_attached->SetTransform(tm);
    }
}

void CStaticObject::CommitPathMotionUpdate() {
    m_pathPosition = PositionRow();
    m_pathSpeed = m_speed216;
    UpdatePathMotionDestAndSpeed();
}

void CStaticObject::CommitAnimMotionUpdate() {
    if (m_animMotion && m_animMotionActive) {
        m_animPosition = PositionRow();
        m_animRotation.SetFromMatrix(m_tm);
    }
}

void CStaticObject::CommitScriptMotionUpdate() {
    m_pathPosition = PositionRow();
    if (m_scriptMove) {
        m_time1b8 -= m_step1d0;
        m_time1b8 = m_time1b8 < 0.0f ? 0.0f : m_time1b8;
    }
    if (m_scriptRotate) {
        m_scriptRotation = m_scriptTargetRotation;
        m_time1bc -= m_step1d4;
        m_time1bc = m_time1bc < 0.0f ? 0.0f : m_time1bc;
    }
    if (m_scriptValue) {
        m_value1b4 += m_step1d8 * m_value1b0;
        m_time1c0 -= m_step1d8;
        m_time1c0 = m_time1c0 < 0.0f ? 0.0f : m_time1c0;
    }
}

void CStaticObject::AttemptUpdate(float dt) {
    if (IsFollowingPath()) {
        AttemptPathMotionUpdate(dt);
    } else if (m_scriptMove || m_scriptRotate || m_scriptValue) {
        if (!m_scriptHold)
            AttemptScriptMotionUpdate(dt);
    } else if (m_animMove) {
        AttemptAnimMotionUpdate(dt);
    }
}
