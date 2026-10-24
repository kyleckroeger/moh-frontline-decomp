// A fragment of player.cpp (0x8009dab4): CPlayerObject::SetUsingMountedMachineGun.
// Mounting (when not already on or mounting a gun): the gun object is kept;
// a hierarchical gun is searched (up to 20 sub-objects) for the sub-object
// whose mesh has the pivot node (CRC 0x48c00881), which becomes the gun,
// with the base flagged attached and the sub-object turning, and the mount
// matrix's second row kept (+1760). From the gun's trigger core: the two
// turn limits (degrees to radians), the weapon (the core's own one with a
// turn rate of byte +112 / 20, else one of two by the weapon CRC), a clip
// size override (kept to restore), then the mount/pivot/muzzle node
// matrices by CRC; the player's transform is saved (relative to a
// hierarchical base), the gun's collision id saved and set to 22, the
// weapon's reload finished when the gun changed, and the closest gun
// trigger looked up (radius 10): when it belongs to this gun it is marked
// used (script event 218 with 1), or event 168 is sent when it is taken.
// Dismounting: the trigger is released (event 218 with 0), the weapon
// stops shooting, the current hand weapon is flagged, and (unless still
// mounting) the player stands at the saved position (plus 0.01 up; for a
// hierarchical gun relative to the base): yaw from the transform, pitch
// clamped to +-1.45, pushed out of contacts by 3 * CONTACT_EPSILON, camera
// at the standing offset; the gun gets its collision id and transform
// back (a hierarchical gun its identity rotation and flags), the clip size
// is restored, the state set to 1 and both mount bits cleared (twice, as
// compiled), with script event 23 for the core's own weapon. The class,
// functions and globals are named by the symbols (CAMERA_STAND_OFFSET and
// g_CameraOffset are file-local in the original and declared extern
// here); the members, the views, the virtual slot names, the core layout
// and the event meanings are inferred. The two stand-up blocks are written
// out as compiled. The rest of the file is in other units.
extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y ;
    }
    return x;
}

template <class T> T MathFunClamp(T, T, T);

extern const float CONTACT_EPSILON;

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) {
        c.x = ax;
        c.y = ay;
        c.z = az;
    }
    CVector3& operator+=(const CVector3& o) {
        this->c.x += o.c.x;
        this->c.y += o.c.y;
        this->c.z += o.c.z;
        return *this;
    }
    CVector3& operator-=(const CVector3& o) {
        this->c.x -= o.c.x;
        this->c.y -= o.c.y;
        this->c.z -= o.c.z;
        return *this;
    }
    CVector3& operator*=(float k) {
        this->c.x *= k;
        this->c.y *= k;
        this->c.z *= k;
        return *this;
    }
    void Normalize() {
        float length = sqrtf(this->c.x * this->c.x + this->c.y * this->c.y + this->c.z * this->c.z);
        if (length != 0.0f) {
            float inv = 1.0f / length;
            this->c.x *= inv;
            this->c.y *= inv;
            this->c.z *= inv;
        }
    }

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

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;
    CMatrix& operator=(const CMatrix&);
    void SetPos(CVector3);
    void BuildTrans(CVector3);
    void BuildRotZ(float);
    void RotateX(float);
    void Multiply(const CMatrix&, const CMatrix&);
    void ToEulerXYZ(float&, float&, float&) const;
    CVector3 GetPos() const { return pos; }
    CVector3 GetFront() const { return front; }

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void Set(float, float, float, float);
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};

extern CVector3 g_CameraOffset;
extern CVector3 CAMERA_STAND_OFFSET;

/* inferred: a mesh node: its matrix and name CRC */
struct MeshNodeView {
    CMatrix m_matrix;
    unsigned int m_crc;
    unsigned char unknown44[12];
};

struct MeshDataView {
    unsigned char unknown00[40];
    MeshNodeView* m_nodes;
    int m_count;
};

struct MeshView {
    unsigned char unknown00[4];
    MeshDataView* m_data;
};

/* inferred: the machine-gun trigger core (limits, flags, weapon CRC, ...) */
struct MOH_core_Struct {
    unsigned char unknown00[16];
    float m_position[3];
    CQuaternion m_rotation;
    unsigned char unknown2c[8];
    unsigned char m_flags34a : 6;
    unsigned char m_ownWeapon : 1;
    unsigned char m_flags34b : 1;
    unsigned char unknown35[5];
    short m_limitA;
    short m_limitB;
    unsigned char unknown3e[10];
    unsigned int m_weaponCrc;
    unsigned char unknown4c[36];
    unsigned char m_turnRate;
    unsigned char m_clipSize;
};

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    class BSObject* bsObject;
};

struct MGTriggerObjectView {
    TriggerObject_struct* waypoint;
    TriggerObject_struct* mechanic;
    unsigned char unknown08[4];
};

extern MGTriggerObjectView g_pMGTriggerObject[32];

class ISceneNode;
int SearchForClosestMachineGun(ISceneNode*, float);
bool IsMGUsed(TriggerObject_struct*);
void MarkMGAsUsed(TriggerObject_struct*, bool);

class BSObject {
public:
    unsigned char unknown00[8];
    TriggerObject_struct* m_trigger;
};

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CHierObject;

class CStaticObject {
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
    virtual int GetCollisionId() const;
    virtual void SetCollisionId(int);
    virtual BSObject* GetScriptObject() const;
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual CHierObject* AsHierObject();
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

    unsigned char unknown004[44];
    MeshView* m_mesh;
};

class CHierObject : public CStaticObject {
public:
    CHierObject* GetSubObject(int);
    void SetRotation(CQuaternion&);

    unsigned char unknown034[1364];
    unsigned char m_attached : 1;
    unsigned char m_turning : 1;
    unsigned char m_flags588 : 6;
    unsigned char unknown589[103];
    CMatrix m_mountMatrix;
};

enum EWeaponShootType {};

/* inferred: the weapon's properties (clip size at +6) */
struct WeaponPropertiesView {
    unsigned char unknown00[6];
    short m_clipSize;
};

class CWeapon {
public:
    void DoneReloading();
    void Shoot(EWeaponShootType, float, float);

    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    unsigned char unknown284[4];
    float m_reloadTime;
};

class CPlayerWeaponObject {
public:
    unsigned char unknown0000[12754];
    unsigned char m_flags31d2a : 1;
    unsigned char m_holstered : 1;
    unsigned char m_flags31d2b : 6;
};

/* inferred: the camera object embedded at +2000 */
class CameraView {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void SetBlend(float);
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
};

/* inferred: a collision contact (normal first, 24 bytes) */
struct CClsnInfo {
    CVector3 m_normal;
    float m_depth;
    void* m_object;
};

class CPlayerObject {
public:
    void SetUsingMountedMachineGun(bool, CStaticObject*);

    unsigned char unknown000[128];
    CMatrix m_tm;
    CMatrix m_cameraTM;
    unsigned char unknown100[336];
    BSObject* m_scriptObject;
    unsigned char unknown254[72];
    float m_yaw;
    float m_pitch;
    unsigned char unknown2a4[4];
    CClsnInfo* m_contacts;
    int unknown2ac;
    int m_contactCount;
    unsigned char unknown2b4[226];
    unsigned char m_onMG : 1;
    unsigned char m_flags396a : 2;
    unsigned char m_mgIsHier : 1;
    unsigned char m_mountingMG : 1;
    unsigned char m_mgHasWeapon : 1;
    unsigned char m_flags396b : 2;
    unsigned char m_flags397a : 6;
    unsigned char m_flag397_02 : 1;
    unsigned char m_flags397b : 1;
    unsigned char unknown398[124];
    float m_limitA;
    float m_limitB;
    float m_value41c;
    float m_value420;
    unsigned char unknown424[56];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[45];
    unsigned char unknown514[180];
    CHierObject* m_mgBase;
    CStaticObject* m_mg;
    CStaticObject* m_lastMG;
    CWeapon* m_mgWeaponA;
    CWeapon* m_mgWeaponB;
    CWeapon* m_mgWeaponC;
    CWeapon* m_weapon;
    unsigned char unknown5e4[12];
    CMatrix m_mgMount;
    CMatrix m_mgPivot;
    CMatrix m_mgSaved;
    int m_mgCollisionId;
    float m_mgTurnRate;
    short m_savedClipSize;
    unsigned char unknown6ba[18];
    int m_state;
    unsigned char unknown6d0[16];
    CVector3 m_mgPos;
    int m_mgIndex;
    unsigned char unknown6f4[4];
    bool m_hasMuzzle;
    unsigned char unknown6f9[7];
    CMatrix m_muzzle;
    unsigned char unknown740[144];
    CameraView m_camera;
};

void CPlayerObject::SetUsingMountedMachineGun(bool use, CStaticObject* mg) {
    if (use && !m_onMG && !m_mountingMG) {
        m_mg = mg;
        m_mgIsHier = mg->AsHierObject() != 0;
        if (m_mgIsHier) {
            CHierObject* hier = mg->AsHierObject();
            CHierObject* sub = 0;
            bool found = false;
            for (int i = 0; i < 20 && !found; i++) {
                sub = hier->GetSubObject(i);
                if (sub) {
                    MeshDataView* data = sub->m_mesh->m_data;
                    MeshNodeView* node = data->m_nodes;
                    int count = data->m_count;
                    for (int j = 0; j < count; j++, node++) {
                        if (node->m_crc == 0x48c00881) {
                            found = true;
                            break;
                        }
                    }
                }
            }
            m_mg = sub;
            m_mgBase = hier;
            m_mgBase->m_attached = 1;
            sub->m_turning = 1;
            CMatrix mount;
            mount = sub->m_mountMatrix;
            m_mgPos = mount.GetFront();
        } else {
            m_mgBase = 0;
        }
        MeshView* mesh = m_mg->m_mesh;
        BSObject* script = m_mg->GetScriptObject();
        MOH_core_Struct* core = script->m_trigger->core;
        m_limitB = 3.1415927f * (float)core->m_limitB / 180.0f;
        m_limitA = 3.1415927f * (float)core->m_limitA / 180.0f;
        m_flag397_02 = 0;
        if (core->m_ownWeapon) {
            m_weapon = m_mgWeaponC;
            m_mgHasWeapon = 1;
            m_mgTurnRate = (float)core->m_turnRate / 20.0f;
        } else {
            m_mgHasWeapon = 0;
            m_mgTurnRate = 1.0f;
            if (core->m_weaponCrc == 0xf6798fe8)
                m_weapon = m_mgWeaponB;
            else
                m_weapon = m_mgWeaponA;
        }
        m_savedClipSize = m_weapon->m_properties->m_clipSize;
        int clip = core->m_clipSize;
        if (clip > 1)
            m_weapon->m_properties->m_clipSize = clip;
        m_mountingMG = 1;
        MeshDataView* data = mesh->m_data;
        int count = data->m_count;
        MeshNodeView* node = data->m_nodes;
        m_hasMuzzle = false;
        for (int i = 0; i < count; i++, node++) {
            unsigned int crc = node->m_crc;
            if (crc == 0xe6e804d5) {
                m_mgMount = node->m_matrix;
            } else if (crc == 0x48c00881) {
                m_mgPivot = node->m_matrix;
            } else if (crc == 0x11bc0f1f) {
                m_muzzle = node->m_matrix;
                m_hasMuzzle = true;
            }
        }
        m_state = 21;
        if (!m_mgIsHier) {
            m_mgCollisionId = m_mg->GetCollisionId();
            m_mg->SetCollisionId(22);
            m_mgSaved = m_tm;
        } else {
            CVector3 offset;
            m_mgBase->GetPosition(offset);
            offset -= m_tm.GetPos();
            m_mgSaved = m_tm;
            m_mgSaved.SetPos(offset);
            m_mgCollisionId = m_mgBase->GetCollisionId();
            m_mgBase->SetCollisionId(22);
        }
        if (m_lastMG != m_mg) {
            m_weapon->DoneReloading();
            m_weapon->m_reloadTime = 0.0f;
        }
        m_lastMG = m_mg;
        m_mgIndex = SearchForClosestMachineGun((ISceneNode*)this, 10.0f);
        if (m_mgIndex >= 0) {
            if (g_pMGTriggerObject[m_mgIndex].mechanic == script->m_trigger) {
                if (IsMGUsed(g_pMGTriggerObject[m_mgIndex].waypoint)) {
                    m_mgIndex = -1;
                    BSObjectTriggerEvent(m_scriptObject, 168, 0, 0, true);
                } else {
                    if (g_pMGTriggerObject[m_mgIndex].waypoint->bsObject)
                        BSObjectTriggerEvent(g_pMGTriggerObject[m_mgIndex].waypoint->bsObject, 218, (void*)1, 0, true);
                    MarkMGAsUsed(g_pMGTriggerObject[m_mgIndex].waypoint, true);
                }
            } else {
                m_mgIndex = -1;
            }
        }
    } else if (!use && (m_onMG || m_mountingMG)) {
        if (m_mgIndex >= 0) {
            MarkMGAsUsed(g_pMGTriggerObject[m_mgIndex].waypoint, false);
            if (g_pMGTriggerObject[m_mgIndex].waypoint->bsObject)
                BSObjectTriggerEvent(g_pMGTriggerObject[m_mgIndex].waypoint->bsObject, 218, 0, 0, true);
            m_mgIndex = -1;
        }
        m_hasMuzzle = false;
        m_weapon->Shoot((EWeaponShootType)1, 0.0f, -1.0f);
        if (m_currentWeapon >= 0)
            m_weapons[m_currentWeapon]->m_holstered = 1;
        if (!m_mgIsHier) {
            if (!m_mountingMG) {
                CVector3 position(0.0f, 0.0f, 0.01f);
                position += m_mgSaved.GetPos();
                float roll;
                m_tm.ToEulerXYZ(m_pitch, roll, m_yaw);
                m_pitch = MathFunClamp<float>(m_pitch, -1.45f, 1.45f);
                m_tm.BuildRotZ(m_yaw);
                m_tm.SetPos(position);
                CVector3 push(0.0f, 0.0f, 0.0f);
                for (CClsnInfo* it = m_contacts; it != m_contacts + m_contactCount; it++)
                    push += it->m_normal;
                push.Normalize();
                push *= 3.0f * CONTACT_EPSILON;
                push += m_tm.GetPos();
                m_tm.SetPos(push);
                CMatrix camera;
                g_CameraOffset = CAMERA_STAND_OFFSET;
                m_cameraTM.BuildTrans(g_CameraOffset);
                m_cameraTM.RotateX(m_pitch);
                camera.Multiply(m_cameraTM, m_tm);
                m_camera.SetTMLocalToWorld(camera);
                m_camera.SetBlend(1.0f);
            }
            m_mg->SetCollisionId(m_mgCollisionId);
            CMatrix tm;
            MOH_core_Struct* core = m_mg->GetScriptObject()->m_trigger->core;
            core->m_rotation.GetMatrix(tm);
            CVector3 position(core->m_position[0], core->m_position[1], core->m_position[2]);
            tm.SetPos(position);
            m_mg->SetTMLocalToWorld(tm);
        } else {
            if (!m_mountingMG) {
                CVector3 position;
                m_mgBase->GetPosition(position);
                position -= m_mgSaved.GetPos();
                position += CVector3(0.0f, 0.0f, 0.01f);
                float roll;
                m_tm.ToEulerXYZ(m_pitch, roll, m_yaw);
                m_pitch = MathFunClamp<float>(m_pitch, -1.45f, 1.45f);
                m_tm.BuildRotZ(m_yaw);
                m_tm.SetPos(position);
                CVector3 push(0.0f, 0.0f, 0.0f);
                for (CClsnInfo* it = m_contacts; it != m_contacts + m_contactCount; it++)
                    push += it->m_normal;
                push.Normalize();
                push *= 3.0f * CONTACT_EPSILON;
                push += m_tm.GetPos();
                m_tm.SetPos(push);
                CMatrix camera;
                g_CameraOffset = CAMERA_STAND_OFFSET;
                m_cameraTM.BuildTrans(g_CameraOffset);
                m_cameraTM.RotateX(m_pitch);
                camera.Multiply(m_cameraTM, m_tm);
                m_camera.SetTMLocalToWorld(camera);
                m_camera.SetBlend(1.0f);
            }
            m_mgBase->SetCollisionId(m_mgCollisionId);
            m_mgBase->m_attached = 0;
            CHierObject* hier = m_mg->AsHierObject();
            hier->m_turning = 0;
            CQuaternion rotation;
            rotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
            hier->SetRotation(rotation);
        }
        m_weapon->m_properties->m_clipSize = m_savedClipSize;
        m_state = 1;
        m_value41c = 0.0f;
        m_value420 = 0.0f;
        m_onMG = 0;
        m_mountingMG = 0;
        m_onMG = 0;
        m_mountingMG = 0;
        if (m_weapon == m_mgWeaponC)
            BSObjectTriggerEvent(m_scriptObject, 23, 0, 0, true);
    }
}
