// A fragment of alarm.cpp (0x80059d5c): the built-in script functions
// SetAlarmOccupy (the alarm whose object's script object is the first
// argument records the calling soldier's script object as its occupant, or
// none when the second argument is clear), UnregisterAlarm (after popping its arguments, the calling object's entry
// is found, its script object, if any, is sent event 247 and the entry is
// erased through the list's inline erase) and RegisterAlarm. The alarm list is given a pool of 15 nodes on first use
// (through the weak reserve, which follows in the image and is only declared
// here), then the calling object is appended with a cleared second word
// through the list's inline push_back. The dwi templates, AlarmInfo and
// g_alarms are named by the symbols; the list layout (as in
// alarm_list_reserve.cpp), push_back, erase, AlarmInfo's members and constructor and the
// built-in record view (as in alarm_builtins.cpp) are inferred; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (as in bsbifunc_attach.cpp). The rest of
// the file is not part of this unit.
inline void* operator new(unsigned long, void* p) { return p; }

struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

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
    virtual void GetPosition(CVector3&) const;
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
    virtual CSoldierObject* AsSoldierObject();
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

class CSoldierObject : public ISceneNode {
};

struct AlarmInfo {
    AlarmInfo(void* object) : m_object(object), m_script(0) {}

    void* m_object;
    BSObject* m_script;
};

namespace dwi {
template <class T>
struct list_node {
    list_node(list_node* prev, list_node* next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}

    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    list_node<T>* alloc_node() {
        list_node<T>* node = m_free;
        m_free = node->m_next;
        return node;
    }
    void reserve(int, char*);
    void erase(list_node<T>* node) {
        list_node<T>* prev = node->m_prev;
        list_node<T>* next = node->m_next;
        prev->m_next = next;
        next->m_prev = prev;
        node->m_prev = 0;
        node->m_next = m_free;
        m_free = node;
        m_count--;
    }
    void push_back(const T& value) {
        list_node<T>* prev = m_head.m_prev;
        list_node<T>* pos = &m_head;
        list_node<T>* node = alloc_node();
        new (node) list_node<T>(0, 0, value);
        m_count++;
        prev->m_next = node;
        node->m_prev = prev;
        node->m_next = pos;
        pos->m_prev = node;
    }

    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short firstArgument;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;
extern dwi::list<AlarmInfo> g_alarms;

void BIFunc_SetAlarmOccupy(int** stack, void* object) {
    bool occupy;
    dwi::list_node<AlarmInfo>* node;
    dwi::list_node<AlarmInfo>* end;
    BSObject* alarm;
    BSObject* script;
    int* args = *stack;
    short count = g_pBuiltInFunctions[g_iCurrentBIFIndex].firstArgument;
    alarm = *(BSObject**)(args - (count - 1));
    occupy = *(args - (count - 2)) != 0;
    script = ((ISceneNode*)object)->AsSoldierObject()->GetScriptObject();
    end = &g_alarms.m_head;
    for (node = g_alarms.m_head.m_next; node != end; node = node->m_next) {
        if (((ISceneNode*)node->m_data.m_object)->GetScriptObject() == alarm) {
            node->m_data.m_script = occupy ? script : 0;
            break;
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_UnregisterAlarm(int** stack, void* object) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    for (dwi::list_node<AlarmInfo>* node = g_alarms.m_head.m_next; node != &g_alarms.m_head; node = node->m_next) {
        if (node->m_data.m_object == object) {
            if (node->m_data.m_script)
                BSObjectTriggerEvent(node->m_data.m_script, 247, 0, node->m_data.m_script, false);
            g_alarms.erase(node);
            break;
        }
    }
}

void BIFunc_RegisterAlarm(int** stack, void* object) {
    if (g_alarms.m_capacity == 0)
        g_alarms.reserve(15, "Alarm Pool");
    g_alarms.push_back(AlarmInfo(object));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
