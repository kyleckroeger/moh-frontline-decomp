// A fragment of scene.cpp (0x800da8f4): CScene::Update. It expires the
// collision exclusions (+280; an entry whose value reaches zero flags both
// players and is erased), runs the nodes' BeginUpdate, ConstrainVelocity,
// UpdateAI and CommitAI passes, then steps the non-bullet nodes through
// AttemptUpdate/DetectSlowCollisions/CommitUpdate in slices of at most
// MAX_TIME_STEP (a collision shortens the slice to the time of impact;
// a slice under 1/64 resets the moving nodes and ends the update), and
// finally moves the bullets with DetectFastCollisions. The queued node
// commands (+304) run after every pass. Each pass is timed into a profile
// bin. CScene, ISceneNode, the As* accessors and the virtual functions are
// named by the symbols (slots from __vt__10ISceneNode); the members, the
// profile bins, the command record and the moving node's reset slot are
// inferred views, as in scene_now.cpp and scene_fast.cpp. The iterator
// destructors are the weak copies placed by scene_veciter_dtor.cpp and
// scene_vecconst_dtor.cpp. The rest of the file is not part of this unit.
extern "C" unsigned long OSGetTick();

struct SProfileBin {
    int m_id;
    int m_values[5];
};

extern SProfileBin g_ProfileBins[31];
extern long g_updateTime;

class CDrawContext;
class CMatrix;
class CVector3;
class CCollision;
class CBullet;
class CPlayerObject;

class CMovingNode {
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
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
};

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
    virtual int GetCollisionId() const;
    virtual void SetCollisionId(int);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CMovingNode* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual void* unknown09c(); // the non-const AsPlayerObject
    virtual CPlayerObject* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual void* AsLight();
    virtual const void* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;

    ISceneNode* m_prev;
    ISceneNode* m_next;
};

class CBullet : public ISceneNode {
};

class CPlayerObject {
public:
    unsigned char unknown000[916];
    unsigned char m_flags394a : 3;
    bool m_flag394 : 1;
    unsigned char m_flags394b : 4;
};

class CCollision {
public:
    ISceneNode* m_node0;
    ISceneNode* m_node1;
    float m_time;
    unsigned char unknown0c[20];
};

struct SExclusionEntry {
    ISceneNode* m_first;
    ISceneNode* m_second;
    float m_value;
};

namespace dwi {
template <class T>
class vec_iter {
public:
    vec_iter() : m_p(0) {}
    vec_iter(T* p) : m_p(p) {}
    ~vec_iter() { m_p = 0; }
    bool operator!=(const vec_iter& other) const { return m_p != other.m_p; }
    vec_iter& operator++() {
        m_p++;
        return *this;
    }
    T* operator->() const { return m_p; }
    T& operator*() const { return *m_p; }

    T* m_p;
};

template <class T>
class vec_const_iter {
public:
    vec_const_iter(const T* p) : m_p(p) {}
    vec_const_iter(const vec_iter<T>& it) : m_p(it.m_p) {}
    ~vec_const_iter() { m_p = 0; }
    bool operator!=(const vec_iter<T>& other) const { return m_p != other.m_p; }
    vec_const_iter& operator++() {
        m_p++;
        return *this;
    }
    const T* operator->() const { return m_p; }
    const T& operator*() const { return *m_p; }

    const T* m_p;
};

template <class T>
class vector {
public:
    vec_iter<T> begin();
    vec_iter<T> end();
    int size() const { return m_size; }

    int m_size;
    int m_capacity;
    T* m_data;
    bool m_owned;
};

template <class T>
class fast_vec {
public:
    T* begin() { return m_data; }
    T* end() { return m_data + m_size; }
    void erase(T* it) {
        T* last = m_data + m_size;
        for (T* src = it + 1; src != last;)
            *it++ = *src++;
        m_size--;
    }
    void clear() { m_size = 0; }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}


template <class T>
inline const T& Min(const T& a, const T& b) {
    return a < b ? a : b;
}

static const float MAX_TIME_STEP = 1.0f;

class CScene {
public:
    struct SModCommand {
        void (CScene::*m_fn)(ISceneNode*);
        ISceneNode* m_node;
    };

    void Update(float);
    bool DetectSlowCollisions(float, float&);
    bool DetectFastCollisions(float);

    void ProcessCommands() {
        m_updating = false;
        for (SModCommand* cmd = m_commands.begin(); cmd != m_commands.end(); cmd++) {
            SModCommand c = *cmd;
            (this->*c.m_fn)(c.m_node);
        }
        m_commands.clear();
    }

    ISceneNode* m_head;
    ISceneNode* m_tail;
    ISceneNode* m_players[4];
    ISceneNode* m_world;
    ISceneNode* m_camera;
    unsigned char unknown020[40];
    bool m_updating;
    unsigned char unknown049[3];
    int m_playerCount;
    unsigned char unknown050[184];
    dwi::vector<CCollision> m_collisions;
    dwi::fast_vec<SExclusionEntry> m_exclusions;
    dwi::fast_vec<SModCommand> m_commands;
    unsigned char unknown148[8];
};

void CScene::Update(float dt) {
    g_ProfileBins[2].m_values[1] -= OSGetTick();
    static bool isCSFWarningEnabled = true;
    if (dt == 0.0f)
        return;
    if (dt > 6.0f)
        dt = 6.0f;
    SExclusionEntry* it = m_exclusions.begin();
    while (it != m_exclusions.end()) {
        if (it->m_value == 0.0f) {
            CPlayerObject* a = it->m_first->AsPlayerObject();
            CPlayerObject* b = it->m_second->AsPlayerObject();
            if (a)
                a->m_flag394 = true;
            if (b)
                b->m_flag394 = true;
            m_exclusions.erase(it);
        } else {
            if (it->m_value > 0.0f) {
                it->m_value -= (int)dt;
                if (it->m_value < 0.0f)
                    it->m_value = 0.0f;
            }
            it++;
        }
    }
    float time;
    ISceneNode* node;
    m_updating = true;
    g_ProfileBins[3].m_values[1] -= OSGetTick();
    for (node = m_head; node; node = node->m_next)
        node->BeginUpdate(dt);
    g_ProfileBins[3].m_values[1] += OSGetTick();
    g_ProfileBins[3].m_values[4]++;
    g_ProfileBins[4].m_values[1] -= OSGetTick();
    for (node = m_head; node; node = node->m_next)
        node->ConstrainVelocity();
    g_ProfileBins[4].m_values[1] += OSGetTick();
    g_ProfileBins[4].m_values[4]++;
    g_ProfileBins[5].m_values[1] -= OSGetTick();
    g_ProfileBins[6].m_values[1] -= OSGetTick();
    for (node = m_head; node; node = node->m_next)
        node->UpdateAI(dt);
    g_ProfileBins[6].m_values[1] += OSGetTick();
    g_ProfileBins[6].m_values[4]++;
    g_ProfileBins[7].m_values[1] -= OSGetTick();
    for (node = m_head; node; node = node->m_next)
        node->CommitAI();
    ProcessCommands();
    g_ProfileBins[5].m_values[1] += OSGetTick();
    g_ProfileBins[5].m_values[4]++;
    g_ProfileBins[7].m_values[1] += OSGetTick();
    g_ProfileBins[7].m_values[4]++;
    g_ProfileBins[8].m_values[1] -= OSGetTick();
    g_ProfileBins[9].m_values[1] -= OSGetTick();
    g_updateTime -= OSGetTick();
    float step = Min(dt, MAX_TIME_STEP);
    int count = (int)(dt / (1.0f / 64.0f)) + 1;
    float cur = 0.0f;
    float end = step;
    float prev = dt;
    for (int i = 1; i <= count; i++) {
        float slice = end - cur;
        if (slice > step) {
            slice = step;
            end = cur + step;
        }
        if (slice < 1.0f / 64.0f)
            goto tooSmall;
        m_updating = true;
        for (node = m_head; node; node = node->m_next) {
            if (!node->AsBullet())
                node->AttemptUpdate(slice);
        }
        float fraction = 1.0f;
        if (DetectSlowCollisions(slice, fraction)) {
            prev = end;
            end = slice * fraction + cur;
        } else {
            if (m_collisions.size()) {
                for (dwi::vec_const_iter<CCollision> c = m_collisions.begin(); c != m_collisions.end(); ++c) {
                    c->m_node0->OnCollision(*c);
                    c->m_node1->OnCollision(*c);
                }
            }
            for (node = m_head; node; node = node->m_next) {
                if (!node->AsBullet())
                    node->CommitUpdate();
            }
            if (dt - end < 1.0f / 64.0f) {
                ProcessCommands();
                break;
            }
            cur = end;
            end = prev;
            prev = dt;
        }
        ProcessCommands();
    }
    g_ProfileBins[9].m_values[1] += OSGetTick();
    g_ProfileBins[9].m_values[4]++;
    g_ProfileBins[10].m_values[1] -= OSGetTick();
    time = dt;
    isCSFWarningEnabled = true;
    m_updating = true;
    for (node = m_head; node; node = node->m_next) {
        if (node->AsBullet())
            node->AttemptUpdate(time);
    }
    if (DetectFastCollisions(time)) {
        for (dwi::vec_const_iter<CCollision> c = m_collisions.begin(); c != m_collisions.end(); ++c) {
            ISceneNode* other = c->m_node1;
            CBullet* bullet = c->m_node0->AsBullet();
            if (!bullet)
                bullet = other->AsBullet();
            else if (other->AsBullet())
                continue;
            bullet->AttemptUpdate(c->m_time);
            bullet->OnCollision(*c);
        }
    }
    for (node = m_head; node; node = node->m_next) {
        if (node->AsBullet())
            node->CommitUpdate();
    }
    ProcessCommands();
    g_ProfileBins[10].m_values[1] += OSGetTick();
    g_ProfileBins[10].m_values[4]++;
    goto done;
tooSmall:
    if (isCSFWarningEnabled)
        isCSFWarningEnabled = false;
    m_updating = true;
    for (node = m_head; node; node = node->m_next) {
        CMovingNode* moving = node->AsMovingNode();
        if (moving)
            moving->unknown0e0();
        node->AttemptUpdate(0.0f);
        node->CommitUpdate();
    }
    ProcessCommands();
done:
    g_updateTime += OSGetTick();
    g_ProfileBins[2].m_values[1] += OSGetTick();
    g_ProfileBins[2].m_values[4]++;
    g_ProfileBins[8].m_values[1] += OSGetTick();
    g_ProfileBins[8].m_values[4]++;
}
