// AIPlayerDoodad.cpp: CAIPlayerDoodad, the AI doodad that drives a player
// object through a CAIFilterPlayerObject. The whole file: the weak
// GetSceneNode, Update, PreUpdate, CleanUp, Init, the destructor (its table
// pointer, then CAIDoodad's destructor) and the constructor (CAIDoodad's
// constructor, its table pointer, the filter pointer at +4 cleared), with
// the class's virtual table and RTTI record.
// The class names come from the mangled symbols; the table's slot order from
// __vt__15CAIPlayerDoodad (CAIDoodad's destructor, GetSceneNode and its two
// fire methods, defined in other files). Only the members these functions
// touch are declared, at their offsets. GetSceneNode is inline in the
// original (a weak symbol), so it is defined __declspec(weak) where the
// original places it; the destructor is the key function.
class ISceneNode;
class CPlayerObject;
class CStaticObject;

class CAIPlayerObject {
public:
    void Init();
};

class CAIFilterObject {
public:
    void LinkGameObject(void*);
};

class CAIFilterPlayerObject : public CAIFilterObject {
public:
    CAIFilterPlayerObject();
    virtual ~CAIFilterPlayerObject();
    void Update(float);
    void PreUpdate();
    void SetOneTimeVars();
    void UpdateManualUpdateVars();

    unsigned char unknown04[4];
    CAIPlayerObject* m_playerObject;
    unsigned char unknown0c[12];
};

class CAIDoodad {
public:
    CAIDoodad();
    virtual ~CAIDoodad();
    virtual ISceneNode* GetSceneNode() const;
    virtual void FireAtCurrentTarget(float);
    virtual void FireMMGAtCurrentTarget(CStaticObject*);
    void Init();
};

class CAIPlayerDoodad : public CAIDoodad {
public:
    virtual ~CAIPlayerDoodad();
    virtual ISceneNode* GetSceneNode() const;
    CAIPlayerDoodad();
    void Update(float);
    void PreUpdate();
    void CleanUp();
    void Init(CPlayerObject*);

    CAIFilterPlayerObject* m_filter;
    unsigned char unknown08[72];
    CPlayerObject* m_player;
};

__declspec(weak) ISceneNode* CAIPlayerDoodad::GetSceneNode() const {
    return (ISceneNode*)m_player;
}

void CAIPlayerDoodad::Update(float time) {
    m_filter->Update(time);
}

void CAIPlayerDoodad::PreUpdate() {
    m_filter->PreUpdate();
}

void CAIPlayerDoodad::CleanUp() {
    delete m_filter;
}

void CAIPlayerDoodad::Init(CPlayerObject* player) {
    if (!m_filter)
        m_filter = new CAIFilterPlayerObject;
    m_filter->LinkGameObject(this);
    m_player = player;
    CAIDoodad::Init();
    CAIFilterPlayerObject* filter = m_filter;
    filter->SetOneTimeVars();
    filter->UpdateManualUpdateVars();
    filter->m_playerObject->Init();
}

CAIPlayerDoodad::~CAIPlayerDoodad() {
}

CAIPlayerDoodad::CAIPlayerDoodad() {
    m_filter = 0;
}
