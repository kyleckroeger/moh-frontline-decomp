// A fragment of hierobject.cpp (0x80098f84): CHierObject's
// StartMotionPlayback (with motion keys, clamps the start and end keys to the
// key count, records them and the script event, takes the key rotation and
// offset from the script rotation and animated position, and starts the
// animation motion) and GetLightVolume (its own light volume when the flag
// at +1416 is set, else its parent's). The classes and functions are named
// by the mangled symbols; CHierObject declares its destructor (defined
// elsewhere) first so its table is not emitted here; CStaticObject declares
// placeholder virtual
// functions named by offset up to GetLightVolume's slot in
// __vt__11CHierObject, the members and flag names are inferred, the part
// table only fills the gap as in hierobject_weak.cpp, and the CVector3 view
// copies through a double pair as in player_weapon.cpp. RegisterBulletDamage
// before these is not matched; the rest of the file is not part of this
// unit.
class BPDLightVolume;

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();

    unsigned char unknown00[4];
};

enum EBSEventEnum {};

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CQuaternion {
public:
    float w;
    float x;
    float y;
    float z;
};

class CStaticObject {
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
    virtual BPDLightVolume* GetLightVolume();
};

class CHierObject : public CStaticObject {
public:
    virtual ~CHierObject();
    void StartMotionPlayback(int, int, EBSEventEnum);
    BPDLightVolume* GetLightVolume();

    struct DamageRange {
        int minimum;
        int maximum;
        float multiplier;
    };

    unsigned char unknown004[300];
    CVector3 m_keyOffset;
    CQuaternion m_keyRotation;
    unsigned char unknown150[48];
    CQuaternion m_scriptRotation;
    unsigned char unknown190[80];
    unsigned char m_flags1e0a : 5;
    unsigned char m_animMove : 1;
    unsigned char m_flags1e0b : 2;
    unsigned char unknown1e1[55];
    int m_keyEvent;
    int m_startKey;
    int m_lastKey;
    int m_keyCount;
    unsigned char unknown228[8];
    int m_key;
    unsigned char unknown234[20];
    void* m_keys;
    unsigned char unknown24c[20];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown264[796];
    unsigned char m_flags580[8];
    unsigned char m_flags588a : 2;
    unsigned char m_ownLight : 1;
    unsigned char m_flags588b : 5;
    unsigned char unknown589[7];
    CHierObject* m_parent;
    unsigned char unknown594[12];
    CHierObject* m_parts[52];
    CVector3 m_animPosition;
    unsigned char unknown680[168];
    DamageRange m_damageRanges[2];
};

void CHierObject::StartMotionPlayback(int frame, int end, EBSEventEnum event) {
    if (!m_keys)
        return;
    if (frame < 0)
        frame = 0;
    if (frame >= m_keyCount)
        frame = m_keyCount - 1;
    if (end < 0)
        end = 0;
    if (end >= m_keyCount)
        end = m_keyCount;
    m_startKey = frame;
    m_key = frame;
    m_lastKey = end;
    m_keyEvent = event;
    m_keyRotation = m_scriptRotation;
    m_keyOffset = m_animPosition;
    m_animMove = true;
}

BPDLightVolume* CHierObject::GetLightVolume() {
    if (m_ownLight)
        return m_lightVolumes.GetVolume();
    return m_parent->GetLightVolume();
}
