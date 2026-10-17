// CSoldierObject, the functions at the start of the file: the AI doodad
// accessor and identity casts, script destruction, head tracking, fading and
// SetBulletEmitter (with the current weapon's emitter: the named emitter's
// index and, from the object's attachment point 0xE6E804D5, the emitter
// matrix; invalid without the weapon emitter or the attachment point). The
// class names come from the mangled symbols. This is an inferred,
// non-virtual view: only the members these functions touch are declared, at
// their offsets (their names are not original), and the virtual table is not
// reproduced. The accessor and casts are inline in the original (weak
// symbols), so they are defined __declspec(weak). The emitter table is read
// through an inferred inline accessor (register order, as in
// Soldier_object_look.cpp) and the loop variables are declared in the order
// that gives the original's registers.
extern "C" int stricmp(const char*, const char*);

class CStaticObject;
class CAIDoodad;

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);

    float m[16];
};

// The object at BSObject+12; only its first virtual slot is called here, and
// its name is unknown (as in bsmachin.cpp).
class BSObjectUserView {
    unsigned char unknown00[12];

public:
    virtual void unknownVirtual0();
};

struct BSObject {
    unsigned char unknown00[12];
    BSObjectUserView* user;
};

class CWeapon {
public:
    unsigned char unknown000[716];
    void* m_emitter;
};

struct SoldierEmitterNameView {
    char name[24];
};

struct SoldierEmitterTableView {
    unsigned char unknown00[16];
    unsigned int count;
    SoldierEmitterNameView* names;
};

struct SoldierAttachPointView {
    CMatrix matrix;
    unsigned int id;
    unsigned char unknown44[12];
};

struct SoldierModelView {
    unsigned char unknown00[40];
    SoldierAttachPointView* attachPoints;
    int attachPointCount;
};

struct SoldierMeshView {
    unsigned char unknown00[4];
    SoldierModelView* model;
};

struct SoldierStaticObjectView {
    unsigned char unknown00[48];
    SoldierMeshView* mesh;
};

class CSoldierObject {
public:
    const CAIDoodad* GetAIDoodad() const;
    const CSoldierObject* AsSoldierObject() const;
    CSoldierObject* AsSoldierObject();
    void Destroy();
    void SetHeadTrans(bool);
    void SetHeadTrackPlayer(bool, int);
    void FadeOut(float);
    void SetBulletEmitter(CStaticObject*, char*);

    unsigned char unknown0000[3300];
    SoldierEmitterTableView* m_emitterTable;
    SoldierEmitterTableView* GetEmitterTable() const { return m_emitterTable; }
    unsigned char unknown0ce8[5800];
    BSObject* m_script;
    unsigned char unknown2394[52];
    float m_headTurn0;
    float m_headTurn1;
    float m_headTurn2;
    unsigned char unknown23d4[3340];
    unsigned char m_aiDoodad[4];
    unsigned char unknown30e4[3804];
    bool m_floatingBoneAttached;
    unsigned char unknown3fc1[3];
    CStaticObject* m_floatingObject;
    unsigned char unknown3fc8[72];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
    unsigned char unknown4078[520];
    bool m_emitterValid;
    unsigned char unknown4281[3];
    int m_emitterIndex;
    unsigned char unknown4288[8];
    CMatrix m_emitterMatrix;
    unsigned char unknown42d0[224];
    bool m_fading;
    unsigned char unknown43b1[3];
    float m_fadeTime;
    float m_fadeDuration;
    unsigned char unknown43bc[4];
    bool m_fadeFlag;
    bool m_headTrackPlayer;
    unsigned char unknown43c2[2];
    int m_headTrackTarget;
    int m_headTrackChanged;
    unsigned char unknown43cc[204];
    signed char m_headTransState;
};

__declspec(weak) const CAIDoodad* CSoldierObject::GetAIDoodad() const {
    return (const CAIDoodad*)m_aiDoodad;
}

__declspec(weak) const CSoldierObject* CSoldierObject::AsSoldierObject() const {
    return this;
}

__declspec(weak) CSoldierObject* CSoldierObject::AsSoldierObject() {
    return this;
}

void CSoldierObject::Destroy() {
    if (m_script && m_script->user)
        m_script->user->unknownVirtual0();
}

void CSoldierObject::SetHeadTrans(bool trans) {
    if (m_headTransState <= 1)
        m_headTransState = trans;
    else if (m_headTransState == 2 && !trans)
        m_headTransState = 3;
}

void CSoldierObject::SetHeadTrackPlayer(bool track, int target) {
    if (track != m_headTrackPlayer) {
        m_headTrackPlayer = track;
        m_headTrackTarget = target;
        m_headTrackChanged = 1;
    }
}

void CSoldierObject::FadeOut(float duration) {
    m_fading = true;
    m_fadeTime = 0.0f;
    m_fadeDuration = duration;
    m_headTurn0 = m_headTurn1 = m_headTurn2 = 0.0f;
    m_fadeFlag = false;
}

void CSoldierObject::SetBulletEmitter(CStaticObject* object, char* name) {
    if (!m_weapons[m_currentWeapon]->m_emitter) {
        m_emitterValid = false;
        return;
    }
    int j;
    int count;
    SoldierEmitterTableView* table = GetEmitterTable();
    unsigned int i;
    for (i = 0; i < table->count; i++) {
        if (stricmp(table->names[i].name, name) == 0)
            break;
    }
    SoldierModelView* model = ((SoldierStaticObjectView*)object)->mesh->model;
    count = model->attachPointCount;
    if (count > 0) {
        SoldierAttachPointView* point = model->attachPoints;
        for (j = 0; j < count; j++, point++) {
            if (point->id == 0xE6E804D5) {
                m_emitterIndex = i;
                m_emitterMatrix = point->matrix;
                m_emitterValid = true;
                break;
            }
        }
        if (j >= count) {
            m_emitterIndex = 0;
            m_emitterValid = false;
        }
    }
}
