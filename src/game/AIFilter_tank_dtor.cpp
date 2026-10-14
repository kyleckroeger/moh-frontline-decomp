// A fragment of AIFilter.cpp (0x8005b068): CAIFilterTankObject's PreUpdate
// (UpdateFromGameObject, then the AI object's PreUpdate), its target-list
// forwards to its CAITankObject (SetTargetMode only stores the mode at
// +1856), ChooseTarget (the tank's choice from the global AI object list; the
// chosen object's owner record (+8) word at +12, or null), the empty
// UpdateManualUpdateVars, SetOneTimeVars (the script objects at +12/+16, then
// AI tuning values from the tank's script properties as for the soldier
// filter, with 7 for the soldier's 4 and a weapon type of 7; then
// UpdateFromGameObject) and its placement operator new; then the
// CAIFilterTankObject destructor (its virtual table pointer, the AI object at
// +8 deleted through its virtual destructor, then CAIFilterObject's table
// pointer). The filter has no delete call: it is constructed in place in its
// owner's memory (see AITankDoodad.cpp), so its operator delete is an inferred
// empty inline. The file name is this project's; the original record is
// AIFilter.cpp. The classes and functions are named by the mangled symbols;
// the views, members and result types not given by the symbols are inferred,
// and the constants are .sdata2 pool entries. CAIFilterObject's destructor is
// defined later in the same original file (AIFilter_caifilterobject_dtor.cpp)
// and inlined here; this fragment cannot hold it, so the view declares it
// inline. The tank filter class declares GetLookDirection (defined elsewhere)
// first, so its global virtual table is not emitted here.
extern "C" {
#include <math.h>
}

enum aifilter_target_mode {};
struct aistatus_match;
class CAIFilterRealPosition;
class CAIFilterRealVector3;

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

/* Inferred: the tank's script properties (members by offset). */
struct TankPropsView {
    unsigned char unknown00[53];
    unsigned char unknown35;
    unsigned char unknown36[2];
    short unknown38;
    unsigned char unknown3a[54];
    unsigned char unknown70;
    unsigned char unknown71;
    unsigned char unknown72;
    unsigned char unknown73;
    unsigned char unknown74[1];
    unsigned char unknown75;
    unsigned char unknown76[3];
    unsigned char unknown79;
    unsigned char unknown7a[4];
    short unknown7e;
    short unknown80;
    short unknown82;
};

struct ScriptDataView {
    unsigned char unknown00[4];
    TankPropsView* props;
};

struct ScriptObjectView {
    unsigned char unknown00[8];
    ScriptDataView* data;
    void* unknown0c;
};

/* Inferred: the record at +16 of the filter; its word at +16. */
struct FilterRecordView {
    unsigned char unknown00[16];
    int m_value;
};

/* Inferred: scene-node views with the table slots used here. */
class CSceneObjectView {
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
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual ScriptObjectView* GetScriptObject() const;
};

/* Inferred: the tank game object (members by offset). */
class CTankView : public CSceneObjectView {
public:
    unsigned char unknown0004[2060];
    CVector3 m_unknown810;
    CVector3 m_unknown820;
};

/* Inferred: the filter's game object (members by offset). */
class CTankDoodadView {
public:
    virtual void unknown008();
    virtual CSceneObjectView* unknown00c();

    unsigned char unknown004[4];
    unsigned int m_flags;
    unsigned int m_previousFlags;
    unsigned char unknown010[64];
    CTankView* m_tank;
};

class CAIObject {
public:
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);

    static CAIObject* GetGlobalListStart();
    void SetLocation(CAIFilterRealPosition&);
    void SetCoordinateAxisDirection(CAIFilterRealVector3&, CAIFilterRealVector3&, CAIFilterRealVector3&);

    unsigned char unknown004[4];
    struct {
        unsigned char unknown00[12];
        void* result;
    }* m_owner;
    float m_unknown00c;
};

/* Inferred: the tank AI object's members by offset. */
class CAITankObject : public CAIObject {
public:
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    void ResetTargetMatchList(aifilter_target_mode);
    CAIObject* ChooseTarget(CAIObject*, float);

    unsigned char m_unknown010[4];
    unsigned char m_unknown014;
    unsigned char m_unknown015;
    short m_unknown016;
    float m_location[4];
    float m_previousLocation[4];
    unsigned char unknown038[104];
    float m_speed;
    unsigned char unknown0a4[4];
    float m_velocity[3];
    unsigned char unknown0b4[16];
    CAIObject* m_target;
    CVector3 m_unknown0c8;
    unsigned char unknown0d8[24];
    CVector3 m_unknown0f0;
    unsigned char unknown100[168];
    unsigned int m_flags;
    unsigned int m_previousFlags;
    unsigned int m_unknown1b0;
    unsigned char unknown1b4[60];
    float m_unknown1f0;
    float m_unknown1f4;
    float m_unknown1f8;
    float m_unknown1fc;
    unsigned char unknown200[4];
    float m_unknown204;
    float m_unknown208;
    unsigned char unknown20c[4];
    float m_unknown210;
    int m_unknown214;
    unsigned char unknown218[1296];
    int m_weaponType;
    unsigned char unknown72c[12];
    float m_unknown738;
    float m_unknown73c;
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
    virtual void* ChooseTarget(float); /* result type inferred */
    virtual ~CAIFilterObject() {}

    CTankDoodadView* m_gameObject;
    CAIObject* m_aiObject;
};

class CAIFilterTankObject : public CAIFilterObject {
public:
    virtual void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);
    virtual void* ChooseTarget(float); /* result type inferred */
    virtual ~CAIFilterTankObject();
    void UpdateFromGameObject();
    void Update(float);
    void PreUpdate();
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    void ResetTargetMatchList(aifilter_target_mode);
    void SetTargetMode(aifilter_target_mode);
    void UpdateManualUpdateVars();
    void SetOneTimeVars();
    static void* operator new(unsigned long, void*);

    static void operator delete(void*) {}

    ScriptObjectView* m_script;
    FilterRecordView* m_scriptData;
};

/* Inferred: whether the target's move goal applies (a target and flag 2). */
inline bool HasMoveGoal(CAITankObject* ai) {
    if (ai->m_target && (ai->m_unknown1b0 & 2) == 2)
        return true;
    return false;
}

void CAIFilterTankObject::PreUpdate() {
    UpdateFromGameObject();
    m_aiObject->PreUpdate();
}

void CAIFilterTankObject::SetupTargetMatchList(aifilter_target_mode mode, aistatus_match* list, int count) {
    ((CAITankObject*)m_aiObject)->SetupTargetMatchList(mode, list, count);
}

void CAIFilterTankObject::ResetTargetMatchList(aifilter_target_mode mode) {
    ((CAITankObject*)m_aiObject)->ResetTargetMatchList(mode);
}

void CAIFilterTankObject::SetTargetMode(aifilter_target_mode mode) {
    ((CAITankObject*)m_aiObject)->m_targetMode = mode;
}

void* CAIFilterTankObject::ChooseTarget(float time) {
    CAITankObject* tank = (CAITankObject*)m_aiObject;
    CAIObject* target = tank->ChooseTarget(CAIObject::GetGlobalListStart(), time);
    return target ? target->m_owner->result : 0;
}

void CAIFilterTankObject::UpdateManualUpdateVars() {
}

void CAIFilterTankObject::SetOneTimeVars() {
    CTankDoodadView* doodad = m_gameObject;
    CAITankObject* ai = (CAITankObject*)m_aiObject;
    m_script = doodad->unknown00c()->GetScriptObject();
    m_scriptData = (FilterRecordView*)m_script->unknown0c;
    TankPropsView* props = doodad->m_tank->GetScriptObject()->data->props;
    if (props->unknown79 == 0)
        ai->m_unknown1f0 = 0.1f;
    else
        ai->m_unknown1f0 = 0.1f * props->unknown79;
    switch (props->unknown75) {
    case 0:
        ai->m_unknown1f8 = 0.5f;
        break;
    case 1:
        ai->m_unknown1f8 = 1.0f;
        break;
    case 2:
        ai->m_unknown1f8 = 2.0f;
        break;
    }
    switch (props->unknown73) {
    case 0:
        ai->m_unknown1f4 = 0.05235988f;
        ai->m_unknown1fc = 1.25f;
        break;
    case 1:
        ai->m_unknown1f4 = 0.034906585f;
        ai->m_unknown1fc = 0.75f;
        break;
    case 2:
        ai->m_unknown1f4 = 0.017453292f;
        ai->m_unknown1fc = 0.333f;
        break;
    }
    ai->m_unknown204 = 0.0625f * props->unknown7e;
    ai->m_unknown208 = 0.0625f * props->unknown80;
    ai->m_unknown210 = 0.0625f * props->unknown82;
    ai->m_unknown214 = props->unknown72;
    ai->m_unknown014 = props->unknown35;
    ai->m_unknown015 = 7;
    ai->m_unknown016 = props->unknown38;
    ai->m_unknown73c = props->unknown70;
    ai->m_unknown738 = ai->m_unknown73c;
    ai->m_weaponType = 7;
    UpdateFromGameObject();
}

void* CAIFilterTankObject::operator new(unsigned long, void* place) {
    return place;
}

CAIFilterTankObject::~CAIFilterTankObject() {
    delete m_aiObject;
}
