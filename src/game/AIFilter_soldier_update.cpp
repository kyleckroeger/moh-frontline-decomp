// A fragment of AIFilter.cpp (0x8005b894): CAIFilterSoldierObject's Update
// (the AI object's Update, then UpdateToGameObject; the AI object's word at
// +12 takes the global filter's time), PreUpdate (UpdateFromGameObject, then
// the AI object's PreUpdate), the target-list forwards to its CAISoldierObject
// (SetTargetMode also stores the mode at +1904 and resets that mode's list)
// and ChooseTarget (the soldier's choice from the global AI object list; the
// result is the chosen object's owner record (+8) word at +12, or null). The
// file name is this project's; the original record is AIFilter.cpp. The
// classes and functions are named by the mangled symbols; the members and the
// result types not given by the symbols are inferred (as in
// AIFilter_soldier_ctor.cpp). The classes declare their destructors (defined
// elsewhere) first, as in their tables, so the global virtual tables are not
// emitted here; CAIObject's virtuals are declared up to Update, as in
// __vt__9CAIObject.
enum aifilter_target_mode {};
struct aistatus_match;
class CAIFilterRealPosition;
class CAIFilterRealVector3;

/* Inferred: the owner record at +8 of an AI object; its word at +12 is the
   choice's result. */
struct AIOwnerView {
    unsigned char unknown00[12];
    void* result;
};

class CAIObject {
public:
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);

    static CAIObject* GetGlobalListStart();

    unsigned char unknown004[4];
    AIOwnerView* m_owner;
    float m_unknown00c;
};

class CAISoldierObject : public CAIObject {
public:
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    void ResetTargetMatchList(aifilter_target_mode);
    CAIObject* ChooseTarget(CAIObject*, float);

    unsigned char unknown010[1888];
    aifilter_target_mode m_targetMode;
};

class CAIFilterGlobal {
public:
    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();

    void* m_gameObject;
    CAIObject* m_aiObject;
};

class CAIFilterSoldierObject : public CAIFilterObject {
public:
    virtual ~CAIFilterSoldierObject();
    void UpdateToGameObject(float);
    void UpdateFromGameObject();
    void Update(float);
    void PreUpdate();
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    void ResetTargetMatchList(aifilter_target_mode);
    void SetTargetMode(aifilter_target_mode);
    void* ChooseTarget(float); /* result type inferred */
};

void CAIFilterSoldierObject::Update(float time) {
    CAIObject* object = m_aiObject;
    object->Update(time);
    UpdateToGameObject(time);
    object->m_unknown00c = g_aigAIFilterGlobalObject.m_time;
}

void CAIFilterSoldierObject::PreUpdate() {
    UpdateFromGameObject();
    m_aiObject->PreUpdate();
}

void CAIFilterSoldierObject::SetupTargetMatchList(aifilter_target_mode mode, aistatus_match* list, int count) {
    ((CAISoldierObject*)m_aiObject)->SetupTargetMatchList(mode, list, count);
}

void CAIFilterSoldierObject::ResetTargetMatchList(aifilter_target_mode mode) {
    ((CAISoldierObject*)m_aiObject)->ResetTargetMatchList(mode);
}

void CAIFilterSoldierObject::SetTargetMode(aifilter_target_mode mode) {
    CAISoldierObject* soldier = (CAISoldierObject*)m_aiObject;
    soldier->m_targetMode = mode;
    soldier->ResetTargetMatchList(mode);
}

void* CAIFilterSoldierObject::ChooseTarget(float time) {
    CAISoldierObject* soldier = (CAISoldierObject*)m_aiObject;
    CAIObject* target = soldier->ChooseTarget(CAIObject::GetGlobalListStart(), time);
    return target ? target->m_owner->result : 0;
}
