// attachobject.cpp: CAttachableObject, the whole file, with its vtable and the RTTI records
// of its chain (IDestructible .. CAttachableObject): the weak destructor and object type and
// the weak copies of CStaticObject's inline script-event trigger, casts, draw-enabled flag
// and attached light (kept from this file); the update hooks (CStaticObject's, then each
// attached object moved along), DetachObject, the three AttachObject overloads and the two
// constructors.
//
// The names come from the mangled symbols; the class chain and the vtable from
// __RTTI__17CAttachableObject and __vt__17CAttachableObject (ISceneNode's and
// IMovingSceneNode's slots from the shared headers, CStaticObject's from +0x124, as
// __vt__13CStaticObject). The bodies of the inline base virtuals follow the matched units
// that keep their weak copies (bsbifunc_distance, bsbifunc_mmg, objcreate_setscript_static,
// bsbifunc_particle, bsbifunc_asweapon, bsbifunc_hierfire).
//
// Inferred, not in any mangled name: the data members (named by offset or role: the mesh at
// +0x30, the matrix at +0x40, the script object at +0x118, the collision id at +0x1dc, the
// flag byte at +0x1e0, the attached light at +0x24c, the ten attachment slots from +0x280),
// the result types, the per-slot inline UpdateAttachedObject (its name too), and the views
// of the mesh's attach points, the script object, CAnimObject and CPlayerObject (file-private
// views at their offsets, not the shared CPlayerObject header). The unit is compiled
// -inline auto,deferred, which emits the functions in reverse source order (and lets
// CommitUpdate inline the later UpdateAttachedObjects), so they are written last to first.
#include "game/ISceneNode.h"
#include "game/IMovingSceneNode.h"

class CThrownObject;
class CWeapon;
class CTankObject;
enum EBSEventEnum {};

/* inferred views */
struct BSObject {
    unsigned char unknown00[16];
    int id;
};

// The mesh's attach points, 80 bytes each (a matrix, then the id at +64), reached through
// the object's mesh (+48) and its data (+4).
struct AttachPointView {
    CMatrix matrix;
    int id;
    unsigned char unknown44[12];
};

struct MeshDataView {
    unsigned char unknown00[40];
    AttachPointView* points;
    int count;
};

class CStaticMesh {
public:
    unsigned char unknown00[4];
    MeshDataView* data;
};

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CStaticObject : public IMovingSceneNode {
public:
    struct MotionFrame;

    CStaticObject();
    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    virtual void MarkForDestruction(int);
    virtual ~CStaticObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId id) { m_collisionId = id; }
    virtual BSObject* GetScriptObject() const { return m_script; }
    virtual void TriggerScriptEvent(int event, void* data, bool flag) {
        BSObjectTriggerEvent(m_script, event, data, 0, flag);
    }
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CStaticObject* AsStaticObject() { return this; }
    virtual const CStaticObject* AsStaticObject() const { return this; }
    virtual bool IsDrawEnabled() const { return m_drawEnabled; }
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const { return m_attachedLight; }
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual BPDLightVolume* GetLightVolume();
    virtual void Init();
    virtual void SetScript(BSObject* script) { m_script = script; }
    virtual int GetObjectType() const;
    virtual void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void ScaleFromScript(float, float, EBSEventEnum);
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual CThrownObject* AsThrownObject() { return 0; }
    virtual CWeapon* AsWeaponObject() { return 0; }
    virtual CTankObject* AsHierTankObject() { return 0; }
    virtual void AttemptScriptMotionUpdate(float);
    virtual void AttemptAnimMotionUpdate(float);
    virtual void AttemptPathMotionUpdate(float);
    virtual void CommitScriptMotionUpdate();
    virtual void CommitAnimMotionUpdate();
    virtual void CommitPathMotionUpdate();

    unsigned char unknown024[12];
    CStaticMesh* m_mesh;
    unsigned char unknown034[12];
    CMatrix m_tm;
    unsigned char unknown080[152];
    BSObject* m_script;
    unsigned char unknown11c[192];
    EClsnId m_collisionId;
    bool m_drawEnabled : 1;
    unsigned char unknown1E0b40 : 6;
    unsigned char m_flag1E0 : 1;
    unsigned char unknown1e1[107];
    CLight* m_attachedLight;
    unsigned char unknown250[48];
};

class CAttachableObject : public CStaticObject {
public:
    virtual ~CAttachableObject() {}
    CAttachableObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CAttachableObject();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void AttemptUpdate(float);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual bool IsVisible(CDrawContext&) const;
    virtual int GetObjectType() const { return 1; }

    void UpdateAttachedObjects();
    bool DetachObject(ISceneNode*);
    int AttachObject(CAnimObject*, int);
    int AttachObject(CPlayerObject*, int, bool);
    int AttachObject(CStaticObject*, int);
    void UpdateAttachedObject(int);

    ISceneNode* m_attached[10];
    unsigned char unknown2a8[8];
    CMatrix m_attachMatrix[10];
    int m_attachFlag[10];
    int m_attachScriptId[10];
};

/* inferred views */
class CAnimObject : public IMovingSceneNode {
public:
    void SetAttached(bool);

    unsigned char unknown024[3868];
    CMatrix m_unknownF40;
    unsigned char unknown0f80[5428];
    bool m_unknown24b4;
};

class CPlayerObject : public IMovingSceneNode {
public:
    void SetHideWeapon(bool);
    /* Inferred inline setters (names not original). */
    void SetFlag02(bool on) { m_flag02 = on; }
    void SetFlag01(bool on) { m_flag01 = on; }

    unsigned char unknown024[882];
    bool m_flag80 : 1;
    bool m_flag40 : 1;
    bool m_flag20 : 1;
    bool m_flag10 : 1;
    bool m_flag08 : 1;
    bool m_flag04 : 1;
    bool m_flag02 : 1;
    bool m_flag01 : 1;
    unsigned char unknown397[937];
    CVector3 m_position;
};

class CScene {
public:
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);

    char data[328];
};

extern CScene g_scene;

inline void CAttachableObject::UpdateAttachedObject(int i) {
    CMatrix tm;
    if (m_attached[i]->GetScriptObject()->id != m_attachScriptId[i]) {
        m_attached[i] = 0;
    } else {
        tm = m_tm;
        tm.Multiply(m_attachMatrix[i], tm);
        switch (m_attachFlag[i]) {
        case 1:
            m_attached[i]->AsStaticObject()->SetTMLocalToWorld(tm);
            break;
        case 2: {
            CAnimObject* anim = m_attached[i]->AsAnimObject();
            if (anim->m_unknown24b4) {
                CMatrix inverse;
                inverse.Inverse(anim->m_unknownF40);
                tm.Multiply(inverse, tm);
            }
            anim->SetTMLocalToWorld(tm);
            break;
        }
        case 3:
            if (m_attached[i]->AsPlayerObject()->m_flag01)
                m_attached[i]->AsPlayerObject()->m_position = tm.GetPos();
            else
                m_attached[i]->AsPlayerObject()->SetTMLocalToWorld(tm);
            break;
        }
    }
}

CAttachableObject::CAttachableObject() {
    for (int i = 0; i < 10; i++)
        m_attached[i] = 0;
}

CAttachableObject::CAttachableObject(CStaticMesh* mesh, int a, int b, int c, int d, int e,
                                     CStaticObject::MotionFrame* frame)
    : CStaticObject(mesh, a, b, c, d, e, frame) {
    for (int i = 0; i < 10; i++)
        m_attached[i] = 0;
}

int CAttachableObject::AttachObject(CStaticObject* object, int id) {
    int index = 0;
    if (!object)
        return 0;
    MeshDataView* data = m_mesh->data;
    AttachPointView* point = data->points;
    for (int i = 0; i < data->count; i++, point++) {
        if (point->id == id) {
            index = i;
            m_attachMatrix[i] = point->matrix;
            break;
        }
    }
    m_attached[index] = object;
    m_attachScriptId[index] = object->GetScriptObject()->id;
    m_attachFlag[index] = 1;
    g_scene.ExcludeCollision(*this, *object, -1);
    return 1;
}

int CAttachableObject::AttachObject(CPlayerObject* player, int id, bool hideWeapon) {
    int index = 0;
    if (!player)
        return 0;
    MeshDataView* data = m_mesh->data;
    AttachPointView* point = data->points;
    for (int i = 0; i < data->count; i++, point++) {
        if (point->id == id) {
            index = i;
            m_attachMatrix[i] = point->matrix;
            break;
        }
    }
    m_attached[index] = player;
    m_attachScriptId[index] = player->GetScriptObject()->id;
    m_attachFlag[index] = 3;
    player->m_flag02 = 1;
    player->m_flag01 = !hideWeapon;
    player->SetHideWeapon(hideWeapon);
    g_scene.ExcludeCollision(*this, *player, -1);
    UpdateAttachedObject(index);
    return 1;
}

int CAttachableObject::AttachObject(CAnimObject* object, int id) {
    int index = 0;
    if (!object)
        return 0;
    MeshDataView* data = m_mesh->data;
    AttachPointView* point = data->points;
    for (int i = 0; i < data->count; i++, point++) {
        if (point->id == id) {
            index = i;
            m_attachMatrix[i] = point->matrix;
            break;
        }
    }
    m_attached[index] = object;
    m_attachScriptId[index] = object->GetScriptObject()->id;
    m_attachFlag[index] = 2;
    object->SetAttached(this);
    g_scene.ExcludeCollision(*this, *object, -1);
    return 1;
}

bool CAttachableObject::DetachObject(ISceneNode* object) {
    bool found = false;
    for (int i = 0; i < 10; i++) {
        if (m_attached[i] == object) {
            found = true;
            if (m_attachFlag[i] == 2) {
                m_attached[i]->AsAnimObject()->SetAttached(false);
            } else if (m_attachFlag[i] == 3) {
                m_attached[i]->AsPlayerObject()->SetFlag02(false);
                m_attached[i]->AsPlayerObject()->SetFlag01(true);
                m_attached[i]->AsPlayerObject()->SetHideWeapon(false);
            }
            m_attached[i] = 0;
            m_attachFlag[i] = 0;
        }
    }
    return found;
}

void CAttachableObject::Draw(CDrawContext& context) {
    CStaticObject::Draw(context);
}

bool CAttachableObject::IsVisible(CDrawContext& context) const {
    return CStaticObject::IsVisible(context);
}

void CAttachableObject::UpdateAttachedObjects() {
    for (int i = 0; i < 10; i++) {
        if (m_attached[i])
            UpdateAttachedObject(i);
    }
}

void CAttachableObject::BeginUpdate(float time) {
    CStaticObject::BeginUpdate(time);
}

void CAttachableObject::UpdateAI(float) {
}

void CAttachableObject::CommitAI() {
}

void CAttachableObject::AttemptUpdate(float time) {
    CStaticObject::AttemptUpdate(time);
}

void CAttachableObject::CommitUpdate() {
    CStaticObject::CommitUpdate();
    UpdateAttachedObjects();
}
