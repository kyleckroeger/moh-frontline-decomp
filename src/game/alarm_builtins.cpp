// A fragment of alarm.cpp; the file name is this project's. Alarm built-in script functions, the first four of the file: setting the
// alarm-activated flag from the first argument, pushing it back,
// AlarmIsAvailable (whether an unoccupied alarm lies within 50 units of the
// calling node) and GetClosestAlarm (the script object of the closest such
// alarm, or 0; both through an inline node-to-node distance helper whose name,
// NodeDistance, is inferred, and the MSL inline sqrtf). The
// globals are named by the symbols; the built-in record view (16 bytes,
// argument count at +10, the first argument's depth at +12) is inferred, as
// in bsbifunc.cpp and bsmachin.cpp; ISceneNode is declared with its virtual
// functions in the order of __vt__10ISceneNode (as in bsbifunc_attach.cpp),
// and the alarm list and AlarmInfo views are as in alarm_register.cpp. The
// rest of the file is not part of this unit.
struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short firstArgument;
    unsigned char unknown0e[2];
};

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
class CAnimObject;
class CSoldierObject;
struct AIDoodadView;
struct BSObject;

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition();

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class ISceneNode {
public:
    enum EVolumeType {};
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CAIFilterRealPosition&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual CAnimObject* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

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
        return y;
    }
    return x;
}

union BSValueView {
    int i;
    BSObject* object;
};

struct AlarmInfo {
    ISceneNode* m_object;
    BSObject* m_script;
};

namespace dwi {
template <class T>
struct list_node {
    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}

extern dwi::list<AlarmInfo> g_alarms;
extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;
extern bool g_alarm_activated;

void BIFunc_SetAlarmActivated(int** stack, void*) {
    BSBuiltinView* builtin = &g_pBuiltInFunctions[g_iCurrentBIFIndex];
    g_alarm_activated = *(*stack - (builtin->firstArgument - 1)) != 0;
    *stack -= builtin->argumentCount;
}

void BIFunc_IsAlarmActivated(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = g_alarm_activated;
}

static inline float NodeDistance(ISceneNode* a, ISceneNode* b) {
    CAIFilterRealPosition pa;
    CAIFilterRealPosition pb;
    a->GetPosition(pa);
    b->GetPosition(pb);
    float dx = pa.x - pb.x;
    float dy = pa.y - pb.y;
    float dz = pa.z - pb.z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

void BIFunc_AlarmIsAvailable(int** stack, void* object) {
    ISceneNode* alarm;
    dwi::list_node<AlarmInfo>* node;
    dwi::list_node<AlarmInfo>* best;
    dwi::list_node<AlarmInfo>* end;
    bool available;
    float bestDistance;
    float distance;
    available = g_alarms.m_count > 0;
    if (available) {
        end = &g_alarms.m_head;
        best = end;
        bestDistance = 50.0f;
        for (node = g_alarms.m_head.m_next; node != end; node = node->m_next) {
            alarm = node->m_data.m_object;
            distance = NodeDistance((ISceneNode*)object, alarm);
            if (distance < bestDistance && node->m_data.m_script == 0) {
                best = node;
                bestDistance = distance;
            }
        }
        if (best == end)
            available = false;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = available;
}

void BIFunc_GetClosestAlarm(int** stack, void* object) {
    ISceneNode* alarm;
    dwi::list_node<AlarmInfo>* node;
    dwi::list_node<AlarmInfo>* best;
    dwi::list_node<AlarmInfo>* end;
    float bestDistance;
    float distance;
    end = &g_alarms.m_head;
    best = end;
    bestDistance = 50.0f;
    for (node = g_alarms.m_head.m_next; node != end; node = node->m_next) {
        alarm = node->m_data.m_object;
        distance = NodeDistance((ISceneNode*)object, alarm);
        if (distance < bestDistance && node->m_data.m_script == 0) {
            best = node;
            bestDistance = distance;
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    if (best != end) {
        BSValueView value;
        value.object = best->m_data.m_object->GetScriptObject();
        *(BSValueView*)*stack = value;
    } else {
        BSValueView value;
        value.i = 0;
        *(BSValueView*)*stack = value;
    }
}
