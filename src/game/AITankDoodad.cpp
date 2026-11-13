// AITankDoodad.cpp: CAITankDoodad, the AI doodad that drives a tank through
// a CAIFilterTankObject placed in the tank's memory. The whole file: the weak
// GetSceneNode, Update, PreUpdate, CleanUp, Init, the destructor (its table
// pointer; the filter object at +4 deleted through its virtual destructor
// and cleared; then CAIDoodad's destructor) and the constructor (CAIDoodad's
// constructor, then its table pointer), with the class's virtual table and
// RTTI record. The class names come from the mangled symbols; the table's
// slot order from __vt__13CAITankDoodad (CAIDoodad's two fire methods are
// defined in another file). Only the members these functions touch are
// declared, at their offsets. GetSceneNode is inline in the original (a weak
// symbol), so it is defined __declspec(weak) where the original places it;
// the destructor is the key function.
class ISceneNode;
class CTankObject;
class CStaticObject;

class CAITankObject {
public:
    void Init();
};

class CAIFilterObject {
public:
    void LinkGameObject(void*);
};

class CAIFilterTankObject : public CAIFilterObject {
public:
    CAIFilterTankObject();
    virtual ~CAIFilterTankObject();
    void* operator new(unsigned long, void*);
    void Update(float);
    void PreUpdate();
    void SetOneTimeVars();
    void UpdateManualUpdateVars();

    unsigned char unknown04[4];
    CAITankObject* m_tankObject;
    unsigned char unknown0c[8];
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

class CAITankDoodad : public CAIDoodad {
public:
    virtual ~CAITankDoodad();
    virtual ISceneNode* GetSceneNode() const;
    CAITankDoodad();
    void Update(float);
    void PreUpdate();
    void CleanUp();
    void Init(CTankObject*);

    CAIFilterTankObject* m_filter;
    unsigned char unknown08[72];
    CTankObject* m_tank;
};

__declspec(weak) ISceneNode* CAITankDoodad::GetSceneNode() const {
    return (ISceneNode*)m_tank;
}

void CAITankDoodad::Update(float time) {
    m_filter->Update(time);
}

void CAITankDoodad::PreUpdate() {
    m_filter->PreUpdate();
}

void CAITankDoodad::CleanUp() {
    delete m_filter;
    m_filter = 0;
}

void CAITankDoodad::Init(CTankObject* tank) {
    m_filter = new ((char*)tank + 2096) CAIFilterTankObject;
    m_filter->LinkGameObject(this);
    m_tank = tank;
    CAIDoodad::Init();
    CAIFilterTankObject* filter = m_filter;
    filter->SetOneTimeVars();
    filter->UpdateManualUpdateVars();
    filter->m_tankObject->Init();
}

CAITankDoodad::~CAITankDoodad() {
    delete m_filter;
    m_filter = 0;
}

CAITankDoodad::CAITankDoodad() {
}
