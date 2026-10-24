// A fragment of bsbifunc.cpp (0x80024448): the script built-in that sets the
// calling node's soldier's mood, and CSoldierObject::SetMood, the weak copy of
// the header inline that follows it. It reads its argument below the script
// stack top and pops the built-in's arguments. SetMood picks per mood a face
// expression morph and weight (blended in at 0.25 per second over the current
// one, or queued as the next when one is already blending; the angry mood uses
// the soldier's own mood index at +17536), the jaw controller's motion type
// (+17472; some moods pick it at random) and the blink timing (minimum and
// maximum interval, close, open and delay times at +17140..). The file name is
// this project's; the original record is bsbifunc.cpp and the built-ins around
// it are not reconstructed. The functions, classes and globals are named by
// the mangled symbols; ISceneNode is declared with its virtual functions in
// the order of __vt__10ISceneNode (AsSoldierObject at +148); the built-in
// record view, the soldier members and the expression and blink helpers
// (inline) are inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
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
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
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


enum EMoodType {};

enum EJawType {};

class CJawController {
public:
    void SetMotionType(EJawType);

    unsigned char unknown00[64];
};

extern "C" int rand();

class CSoldierObject {
public:
    /* inferred: blend the face towards expression morph 'index' at 'weight' */
    void SetExpression(int index, float weight) {
        if (m_nextExpression >= 0) {
            m_weightMax = weight;
            m_weightRate = 0.0f;
            m_weight = weight;
            m_expression = index;
            m_nextWeightMax = 0.0f;
            m_nextWeightRate = 0.0f;
            m_nextWeight = 0.0f;
            m_nextExpression = -1;
        } else if (m_expression == -1) {
            m_weightMax = weight;
            m_weightRate = 0.25f * m_weightMax;
            m_weight = 0.0f;
            m_expression = index;
        } else if (m_expression != index) {
            m_weightRate = 0.25f * -m_weight;
            m_nextWeightMax = weight;
            m_nextWeightRate = 0.25f * m_nextWeightMax;
            m_nextWeight = 0.0f;
            m_nextExpression = index;
        }
    }
    /* inferred: the blink timing */
    void SetBlink(float minimum, float maximum, float close, float open, float delay) {
        m_blinkMin = minimum;
        m_blinkMax = maximum;
        m_blinkClose = close;
        m_blinkDelay = delay;
        m_blinkOpen = open;
        m_blinkTimer = 0.0f;
    }
    void SetMood(EMoodType mood) {
        switch (mood) {
        case 6:
            SetExpression(8, 1.0f);
            m_jaw.SetMotionType((EJawType)7);
            SetBlink(1.0f, 2.0f, 0.017f, 0.017f, 0.017f);
            break;
        case 0:
            SetExpression(4, 1.0f);
            if (rand() % 100 < 50)
                m_jaw.SetMotionType((EJawType)5);
            else
                m_jaw.SetMotionType((EJawType)6);
            SetBlink(2.0f, 4.0f, 0.1f, 0.1f, 0.1f);
            break;
        case 1:
            if (rand() % 100 < 50) {
                SetExpression(3, 1.0f);
                if (rand() % 100 < 50)
                    m_jaw.SetMotionType((EJawType)5);
                else
                    m_jaw.SetMotionType((EJawType)6);
            } else {
                SetExpression(1, 1.0f);
                if (rand() % 100 < 30)
                    m_jaw.SetMotionType((EJawType)6);
                else
                    m_jaw.SetMotionType((EJawType)5);
            }
            SetBlink(1.0f, 2.0f, 0.017f, 0.017f, 0.017f);
            break;
        case 2: {
            SetExpression(m_mood, 1.0f);
            int roll = rand() % 100;
            if (roll < 20)
                m_jaw.SetMotionType((EJawType)3);
            else if (roll < 60)
                m_jaw.SetMotionType((EJawType)2);
            else
                m_jaw.SetMotionType((EJawType)6);
            SetBlink(0.5f, 2.0f, 0.017f, 0.017f, 0.017f);
            break;
        }
        case 3:
            if (rand() % 100 < 50)
                SetExpression(11, 1.0f);
            else
                SetExpression(8, 0.7f);
            m_jaw.SetMotionType((EJawType)1);
            SetBlink(0.0f, 0.0f, 0.0f, 86400.0f, 0.0f);
            break;
        case 4:
            SetExpression(8, 0.7f);
            SetBlink(0.0f, 0.0f, 86400.0f, 0.5f, 0.0f);
            m_jaw.SetMotionType((EJawType)0);
            break;
        case 5: {
            int roll = rand() % 100;
            if (roll < 20) {
                SetExpression(8, 0.7f);
                SetBlink(0.0f, 0.0f, 0.0f, 86400.0f, 0.0f);
                m_jaw.SetMotionType((EJawType)4);
            } else if (roll < 60) {
                SetExpression(8, 0.7f);
                SetBlink(0.0f, 0.0f, 86400.0f, 0.5f, 0.0f);
                m_jaw.SetMotionType((EJawType)0);
            } else {
                SetExpression(11, 0.8f);
                SetBlink(0.0f, 0.0f, 0.0f, 86400.0f, 0.0f);
                m_jaw.SetMotionType((EJawType)1);
            }
            break;
        }
        default:
            SetExpression(0, 1.0f);
            m_jaw.SetMotionType((EJawType)0);
            SetBlink(2.0f, 4.0f, 0.1f, 0.1f, 0.1f);
            break;
        }
    }

    unsigned char unknown0000[17108];
    int m_expression;
    int m_nextExpression;
    float m_weight;
    float m_nextWeight;
    float m_weightMax;
    float m_nextWeightMax;
    float m_weightRate;
    float m_nextWeightRate;
    float m_blinkTimer;
    float m_blinkMin;
    float m_blinkMax;
    float m_blinkClose;
    float m_blinkOpen;
    float m_blinkDelay;
    unsigned char unknown430c[308];
    CJawController m_jaw;
    int m_mood;
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_SetMood(int** stack, void* object) {
    EMoodType mood = (EMoodType)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ((ISceneNode*)object)->AsSoldierObject()->SetMood(mood);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
