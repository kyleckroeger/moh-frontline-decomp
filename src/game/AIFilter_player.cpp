// A fragment of AIFilter.cpp (0x8005abbc): CAIFilterPlayerObject's PreUpdate
// (UpdateFromGameObject, then the AI object's PreUpdate), the empty
// UpdateManualUpdateVars, SetOneTimeVars (the script objects at +12/+16, then
// fixed and script-property values; then UpdateFromGameObject), the
// destructor (the AI object deleted through its virtual destructor). The file name is this project's; the original record is AIFilter.cpp. The
// classes and functions are named by the mangled symbols; the views, members
// and result types not given by the symbols are inferred, and the constants
// are .sdata2 pool entries. The player filter class declares GetLookDirection
// (defined elsewhere) first, so its global virtual table is not emitted here.
extern "C" {
#include <math.h>
}

enum aifilter_target_mode {};
struct aistatus_match;
class CAIFilterRealPosition {
public:
    float x;
    float y;
    float z;
};
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

/* Inferred: the player's script properties (members by offset). */
struct PlayerPropsView {
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
    PlayerPropsView* props;
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

/* Inferred: the player game object. */
class CPlayerView : public CSceneObjectView {
};

/* Inferred: the filter's game object (members by offset). */
class CPlayerDoodadView {
public:
    virtual void unknown008();
    virtual CSceneObjectView* unknown00c();

    unsigned char unknown004[4];
    unsigned int m_flags;
    unsigned int m_previousFlags;
    unsigned char unknown010[64];
    CPlayerView* m_player;
};

class CAIFilterObject;

class CAIObject {
public:
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const;

    static CAIObject* GetGlobalListStart();
    void SetLocation(float, float, float);
    void SetCoordinateAxisDirection(float, float, float, float, float, float, float, float, float);
    bool IsObjectInFieldOfView(CAIFilterRealPosition&, float, float, bool);

    CAIObject* m_next;
    CAIFilterObject* m_filter;
    float m_unknown00c;
    unsigned char unknown010[4];
    char m_unknown014;
    unsigned char m_unknown015;
    short m_unknown016;
    CAIFilterRealPosition m_location;
};

/* Inferred: the player AI object's members by offset (1856 bytes, from the
   allocation). */
class CAIPlayerObject : public CAIObject {
public:
    CAIPlayerObject(void*);

    unsigned char unknown024[4];
    float m_previousLocation[4];
    unsigned char unknown038[104];
    float m_speed;
    unsigned char unknown0a4[4];
    float m_velocity[3];
    unsigned char unknown0b4[16];
    CAIObject* m_target;
    unsigned char unknown0c8[224];
    unsigned int m_flags;
    unsigned int m_previousFlags;
    unsigned char unknown1b0[100];
    int m_unknown214;
    unsigned char unknown218[1296];
    unsigned char unknown728[16];
    float m_unknown738;
    float m_unknown73c;
};

/* Inferred: the field-of-view values of an AI object (+1768 angle, +1840 a
   halving flag), as in AIObject_visionseek.cpp. */
struct AIVisionView {
    unsigned char unknown000[1768];
    float m_angle;
    unsigned char unknown6ec[68];
    bool m_halved;
};

struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

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

    CPlayerDoodadView* m_gameObject;
    CAIObject* m_aiObject;
    ScriptObjectView* m_script;
};

class CAIFilterPlayerObject : public CAIFilterObject {
public:
    virtual void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);
    virtual ~CAIFilterPlayerObject();
    CAIFilterPlayerObject();
    void UpdateFromGameObject();
    void Update(float);
    void PreUpdate();
    void UpdateManualUpdateVars();
    void SetOneTimeVars();

    FilterRecordView* m_scriptData;
    float m_nextCheck;
};

void CAIFilterPlayerObject::PreUpdate() {
    UpdateFromGameObject();
    m_aiObject->PreUpdate();
}

void CAIFilterPlayerObject::UpdateManualUpdateVars() {
}

void CAIFilterPlayerObject::SetOneTimeVars() {
    CPlayerDoodadView* doodad = m_gameObject;
    CAIPlayerObject* ai = (CAIPlayerObject*)m_aiObject;
    m_script = doodad->unknown00c()->GetScriptObject();
    m_scriptData = (FilterRecordView*)m_script->unknown0c;
    PlayerPropsView* props = doodad->m_player->GetScriptObject()->data->props;
    ai->m_unknown214 = 10;
    ai->m_unknown014 = props->unknown35;
    ai->m_unknown015 = 5;
    ai->m_unknown016 = props->unknown38;
    ai->m_unknown73c = props->unknown70;
    ai->m_unknown738 = ai->m_unknown73c;
    UpdateFromGameObject();
}

CAIFilterPlayerObject::~CAIFilterPlayerObject() {
    delete m_aiObject;
}
