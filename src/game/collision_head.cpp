// The start of collision.cpp (0x800c74c8): CCollision::ClearContacts returns
// every contact to the file's contact pool (g_contactPool, file-local in the
// original and defined elsewhere) and counts them off. CCollision::Test asks
// the first node's volume (of the kind the collision selects) to test the
// second's: in single mode once; otherwise it bisects the time step (at most
// MAX_NUM_LOOPS halvings down to TIME_STEP_EPSILON) until a contact is found,
// reporting the fraction of the step reached, and when none converges it
// resets both nodes to the start of the step, excludes the pair from further
// collision and clears the contacts. CCollision, SClsnContact, dwi::CFactory,
// the statics and the called functions are named by the symbols; the members,
// the scene node's virtual slots and the result codes are inferred (see
// collision_contacts.cpp). The rest of the file is not part of this unit.
struct SClsnContact {
    unsigned char unknown00[44];
    SClsnContact* m_next;
};

namespace dwi {
template <class T, class P>
class CFactory {
public:
    void Free(P p) {
        m_used--;
        *(P*)p = m_free;
        m_free = p;
    }

    T* m_memory;
    P m_free;
    int m_capacity;
    int m_used;
    bool m_owned;
};
}

extern dwi::CFactory<SClsnContact, SClsnContact*> g_contactPool;

extern "C" int sprintf(char*, const char*, ...);

class CCollision;

class IVolume {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
};

class CAnimController {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual void vf9c();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual void vfbc();
    virtual void vfc0();
    virtual void vfc4();
    virtual void vfc8();
    virtual void vfcc();
    virtual void vfd0();
    virtual void vfd4();
    virtual void vfd8();
    virtual void vfdc();
    virtual void Reset();
};

class CPlayerObject {
public:
    unsigned char unknown000[916];
    unsigned char flag394a : 1;
    unsigned char flag394b : 1;
    unsigned char flag394c : 1;
    unsigned char m_collided : 1;
};

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void Draw(void*);
    virtual IVolume* GetLocalBoundingVolume(int) const;
    virtual IVolume* GetWorldBoundingVolume(int) const;
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual CAnimController* GetAnimController();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual CPlayerObject* GetPlayerObject();
};

class CScene {
public:
    unsigned char unknown00[64];
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);
};

extern CScene g_scene;
extern const float TIME_STEP_EPSILON;

class CCollision {
public:
    void ClearContacts();
    int Test(float&);

    ISceneNode* m_first;
    ISceneNode* m_second;
    float m_time;
    float m_scale;
    unsigned char m_scaleTime : 1;
    unsigned char m_single : 1;
    unsigned char m_volume : 2;
    int data14;
    SClsnContact* m_contacts;
    int m_numContacts;
};

void CCollision::ClearContacts() {
    while (m_contacts) {
        SClsnContact* next = m_contacts->m_next;
        g_contactPool.Free(m_contacts);
        m_contacts = next;
        m_numContacts--;
    }
}

int CCollision::Test(float& fraction) {
    static bool isCSFWarningEnabled = true;
    int volumes[3] = {0, 1, 2};
    int volume = volumes[m_volume];
    IVolume* first = m_first->GetWorldBoundingVolume(volume);
    IVolume* second = m_second->GetWorldBoundingVolume(volume);
    int result = 0;
    if (m_single) {
        result = first->TestCollision(*second, *this, false);
    } else {
        float start = m_time;
        float lo = 0.0f;
        float hi = start;
        static const int MAX_NUM_LOOPS = (int)(1.0f / TIME_STEP_EPSILON) + 1;
        int i;
        for (i = 1; i <= MAX_NUM_LOOPS; i++) {
            if (m_time < TIME_STEP_EPSILON)
                break;
            int hit = first->TestCollision(*second, *this, i == 1);
            if (hit > result)
                result = hit;
            switch (hit) {
            case 2:
                fraction = m_time / start;
                if (m_scaleTime)
                    m_time *= m_scale;
                goto found;
            case 1:
                lo = m_time;
                m_scaleTime = 0;
                if (i == 1)
                    goto found;
                break;
            case 3:
                hi = m_time;
                ClearContacts();
                break;
            }
            m_time = 0.5f * (lo + hi);
            m_first->AttemptUpdate(m_time);
            first = m_first->GetWorldBoundingVolume(volume);
            m_second->AttemptUpdate(m_time);
            second = m_second->GetWorldBoundingVolume(volume);
        }
        if (isCSFWarningEnabled) {
            char message[260];
            sprintf(message, "\n\tContact search failed (inner loop): %d of %d iterations", i, MAX_NUM_LOOPS);
            isCSFWarningEnabled = false;
        }
        CAnimController* controller = m_first->GetAnimController();
        if (controller)
            controller->Reset();
        m_first->AttemptUpdate(0.0f);
        m_first->CommitUpdate();
        controller = m_second->GetAnimController();
        if (controller)
            controller->Reset();
        m_second->AttemptUpdate(0.0f);
        m_second->CommitUpdate();
        g_scene.ExcludeCollision(*m_first, *m_second, 10);
        CPlayerObject* player1 = m_first->GetPlayerObject();
        CPlayerObject* player2 = m_second->GetPlayerObject();
        if (player1)
            player1->m_collided = 0;
        if (player2)
            player2->m_collided = 0;
        data14 = 0;
        m_scale = 1.0f;
        ClearContacts();
        return 1;
    found:
        isCSFWarningEnabled = true;
    }
    return result;
}
