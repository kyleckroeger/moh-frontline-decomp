// A fragment of AIFilter.cpp (0x8005b494): CAIFilterSoldierObject's
// UpdateToGameObject (the AI object's flag words copied to the game object;
// when flag 2 is set it is cleared, the AI location takes the destination,
// the animation object's velocity becomes the move to it over the time step
// (the height kept from the object's position) and the yaw change is applied
// when flagged; then the move target given to the animation object: the
// avoidance (+1672) or move target (+80) in modes other than 0 and 5, else
// the +240 position when its flag allows), UpdateFromGameObject (the flags
// kept and masked, the location, axes, velocity, speed (SDK sqrtf), upper
// location and two more animation-object values read back; the axes are
// fetched a second time with the results unused, as in the original), then
// CAIFilterSoldierObject's Update
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

/* Inferred: the game object's animation object (table slots up to
   GetUpward, as in ISceneNode; members by offset). */
class CAnimObject {
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

    float GetYaw() const;
    void SetYaw(float);
    void SetMoveTarget(const CVector3&);

    unsigned char unknown0004[9156];
    CVector3 m_velocity;
    unsigned char unknown23d8[7096];
    CVector3 m_upperLocation;
    CVector3 m_unknown3fa0;
};

/* Inferred: the filter's game object (members by offset). */
struct FilterGameObjectView {
    unsigned char unknown00[8];
    unsigned int m_flags;
    unsigned int m_previousFlags;
    unsigned char unknown10[64];
    CAnimObject* m_animObject;
};

/* Inferred: the record at +16 of the filter; its word at +16. */
struct FilterRecordView {
    unsigned char unknown00[16];
    int m_value;
};


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
    void SetLocation(CAIFilterRealPosition&);
    void SetCoordinateAxisDirection(CAIFilterRealVector3&, CAIFilterRealVector3&, CAIFilterRealVector3&);

    unsigned char unknown004[4];
    AIOwnerView* m_owner;
    float m_unknown00c;
};

class CAISoldierObject : public CAIObject {
public:
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    void ResetTargetMatchList(aifilter_target_mode);
    CAIObject* ChooseTarget(CAIObject*, float);
    void SetUpperLocation(CAIFilterRealPosition&);

    unsigned char unknown010[8];
    float m_location[4];
    float m_previousLocation[4];
    float m_destination[4];
    float m_yawChange;
    bool m_changeYaw;
    unsigned char unknown04d[3];
    CVector3 m_moveTarget;
    unsigned char unknown060[64];
    float m_speed;
    unsigned char unknown0a4[4];
    float m_velocity[3];
    unsigned char unknown0b4[60];
    CVector3 m_unknown0f0;
    unsigned char unknown100[168];
    unsigned int m_flags;
    unsigned int m_previousFlags;
    unsigned int m_unknown1b0;
    unsigned char unknown1b4[100];
    int m_mode;
    unsigned char unknown21c[200];
    bool m_unknown2e4;
    unsigned char unknown2e5[931];
    CVector3 m_unknown688;
    unsigned char unknown698[160];
    float m_unknown738[3];
    unsigned char unknown744[36];
    float m_unknown768;
    unsigned char unknown76c[4];
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

    FilterGameObjectView* m_gameObject;
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

    unsigned char unknown0c[4];
    FilterRecordView* m_record;
};

void CAIFilterSoldierObject::UpdateToGameObject(float time) {
    {
        FilterGameObjectView* game = m_gameObject;
        CAISoldierObject* ai = (CAISoldierObject*)m_aiObject;
        game->m_flags = ai->m_flags;
        game->m_previousFlags = ai->m_previousFlags;
    }
    FilterGameObjectView* game = m_gameObject;
    CAISoldierObject* ai = (CAISoldierObject*)m_aiObject;
    if (ai->m_flags & 2) {
        ai->m_flags &= ~2;
        float z = ai->m_destination[2];
        float y = ai->m_destination[1];
        float x = ai->m_destination[0];
        ai->m_location[0] = x;
        ai->m_location[1] = y;
        ai->m_location[2] = z;
        CVector3 position;
        game->m_animObject->GetPosition(position);
        ai->m_location[2] = position.z;
        float scale = 1.0f / time;
        position.x = ai->m_location[0] - position.x;
        position.y = ai->m_location[1] - position.y;
        position.z = ai->m_location[2] - position.z;
        position.x *= scale;
        position.y *= scale;
        position.z *= scale;
        CVector3 velocity;
        velocity = position;
        game->m_animObject->m_velocity = velocity;
        if (ai->m_changeYaw)
            game->m_animObject->SetYaw(ai->m_yawChange + game->m_animObject->GetYaw());
    }
    if (ai->m_mode != 0 && ai->m_mode != 5) {
        if (ai->m_unknown2e4)
            game->m_animObject->SetMoveTarget(ai->m_unknown688);
        else
            game->m_animObject->SetMoveTarget(ai->m_moveTarget);
    } else if ((ai->m_unknown1b0 & 2) == 2) {
        game->m_animObject->SetMoveTarget(ai->m_unknown0f0);
    }
}

void CAIFilterSoldierObject::UpdateFromGameObject() {
    {
        CAISoldierObject* ai = (CAISoldierObject*)m_aiObject;
        ai->m_previousFlags = ai->m_flags;
        ai->m_flags &= 3;
    }
    FilterGameObjectView* game = m_gameObject;
    CAISoldierObject* ai = (CAISoldierObject*)m_aiObject;
    CVector3 rightward;
    CVector3 forward;
    CVector3 upward;
    CVector3 position;
    CVector3 other;
    CVector3 upper;
    game->m_animObject->GetPosition(position);
    ai->SetLocation((CAIFilterRealPosition&)position);
    game->m_animObject->GetRightward(rightward);
    game->m_animObject->GetForward(forward);
    game->m_animObject->GetUpward(upward);
    ai->SetCoordinateAxisDirection((CAIFilterRealVector3&)rightward, (CAIFilterRealVector3&)forward, (CAIFilterRealVector3&)upward);
    ai->m_velocity[0] = ai->m_location[0] - ai->m_previousLocation[0];
    ai->m_velocity[1] = ai->m_location[1] - ai->m_previousLocation[1];
    ai->m_velocity[2] = ai->m_location[2] - ai->m_previousLocation[2];
    float z = ai->m_location[2];
    float y = ai->m_location[1];
    float x = ai->m_location[0];
    ai->m_previousLocation[0] = x;
    ai->m_previousLocation[1] = y;
    ai->m_previousLocation[2] = z;
    ai->m_speed = sqrtf(ai->m_velocity[0] * ai->m_velocity[0] + ai->m_velocity[1] * ai->m_velocity[1] + ai->m_velocity[2] * ai->m_velocity[2]);
    upper = game->m_animObject->m_upperLocation;
    position = upper;
    ai->SetUpperLocation((CAIFilterRealPosition&)position);
    game->m_animObject->GetRightward(rightward);
    game->m_animObject->GetForward(forward);
    game->m_animObject->GetUpward(upward);
    ai->m_unknown768 = m_record->m_value;
    other = game->m_animObject->m_unknown3fa0;
    position = other;
    ai->m_unknown738[0] = other.x;
    ai->m_unknown738[1] = other.y;
    ai->m_unknown738[2] = other.z;
}

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
