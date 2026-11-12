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
        return y ;
    }
    return x;
}

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
class CAIObject;
class CAIFilterObject;
struct BSObjectView;

class CVector3 {
public:
    void Set(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }
    CVector3& operator=(const CVector3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    CVector3& operator-=(const CVector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    float LengthSq() const { return x * x + y * y + z * z; }
    float Length() const { return sqrtf(x * x + y * y + z * z); }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CAIFilterRealPosition : public CVector3 {
public:
    CAIFilterRealPosition& operator=(CVector3 v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
};

struct AIDoodadView {
    unsigned char unknown00[4];
    CAIFilterObject* filter;
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
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObjectView* GetScriptObject() const;
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
    virtual void* AsAnimObject();
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
};

struct ScriptDataView {
    int value;
    unsigned char unknown04[168];
    float x;
    float y;
    float z;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual ScriptDataView* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    unsigned char unknown1c[24];
    unsigned char flags;
    unsigned char team;
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[50];
    unsigned char health;
};

struct TriggerObject_struct {
    unsigned int flags;
    TriggerCoreView* core;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

class CAIObject {
public:
    bool IsObjectInLineOfSight(CAIFilterRealPosition&, float, bool);
    bool IsObjectInFieldOfView(CAIFilterRealPosition&, float, float, bool);
    bool IsObjectReachable(const CAIFilterRealPosition&);

    unsigned char unknown000[80];
    CVector3 position;
    unsigned char unknown060[240];
    CVector3 lookPoint;
    unsigned char unknown15c[1489];
    bool lookPointSet;
};

class CAIFilterObject {
public:
    BSObjectView* GetTargetScript() const;
    void GetTargetGuessPosition(CAIFilterRealPosition&);

    unsigned char unknown00[8];
    CAIObject* m_ai;
};

class CAIFilterGlobal {
public:
    static bool CheckVisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&, CAIFilterObject*);
};

class CScene {
public:
    ISceneNode* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;
extern BSObjectView* g_pBSObject;
extern bool g_bInMultiplayerMode;

float MathFunGetPanAngleDiffNoRoll(CVector3*, CVector3*, CVector3*);
float MathFunGetTiltAngleDiffNoRoll(CVector3*, CVector3*, CVector3*, CVector3*);
float MathFunAtan2F(float, float);
float MathFunNormalizeAngleNegativePiToPi(float);
void MathFunRotateAboutZ(float*, float*, float);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

static inline CAIFilterObject* GetFilter(ISceneNode* node) {
    CAIFilterObject* filter = 0;
    if (node)
        filter = node->GetAIDoodad()->filter;
    return filter;
}

#define ARG(n) (*stack - (count - (n)))
#define CheckVisionLine CAIFilterGlobal::CheckVisionLineToEnvironment

// Draft workaround (not landable as is): ArgObject only steers registers; written out as
// '*(BSObjectView**)ARG(2)' the function is 194 lines off (filter/ARG(2) offset ranks).
static inline BSObjectView* ArgObject(int** stack, int count, int n) {
    return *(BSObjectView**)(*stack - (count - n));
}

void BIFunc_GetObjInfo(int** stack, void* object) {
    ISceneNode* self;
    CAIFilterObject* filter;
    BSGO_Basic* basic;
    int which;
    ISceneNode* node;
    bool isFloat;
    int iresult;
    BSObjectView* target;
    float fresult;
    float value;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    filter = 0;
    iresult = 0;
    basic = 0;
    node = 0;
    fresult = 0.0f;
    target = ArgObject(stack, count, 2);
    value = *(float*)ARG(3);
    self = (ISceneNode*)object;
    isFloat = false;
    which = *ARG(1);
    CVector3 selfPos;
    CVector3 targetPos;
    CAIFilterRealPosition pos;

    switch (which) {
    case 1:
        iresult = (int)g_scene.GetPlayer(0)->GetScriptObject();
        break;
    case 2:
        iresult = (int)g_pBSObject;
        break;
    case 3:
        if (target) {
            if (self)
                filter = self->GetAIDoodad()->filter;
            else
                filter = 0;
            iresult = (int)filter->GetTargetScript();
        }
        break;
    case 10:
        if (target)
            basic = target->user;
        if (basic)
            node = (0, basic->GetSceneNode());
        if (node) {
            if (self) {
                self->GetPosition(selfPos);
            } else if (g_pBSObject && g_pBSObject->trigger && g_pBSObject->trigger->core) {
                TriggerCoreView* core = g_pBSObject->trigger->core;
                selfPos.Set(core->x, core->y, core->z);
            }
            node->GetPosition(targetPos);
            isFloat = true;
            float dx = selfPos.x - targetPos.x;
            float dy = selfPos.y - targetPos.y;
            fresult = sqrtf(dx * dx + dy * dy);
        }
        break;
    case 11:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            if (self) {
                self->GetPosition(selfPos);
            } else if (g_pBSObject && g_pBSObject->trigger && g_pBSObject->trigger->core) {
                TriggerCoreView* core = g_pBSObject->trigger->core;
                selfPos.Set(core->x, core->y, core->z);
            }
            node->GetPosition(targetPos);
            isFloat = true;
            selfPos -= targetPos;
            fresult = sqrtf(selfPos.x * selfPos.x + selfPos.y * selfPos.y + selfPos.z * selfPos.z);
        }
        break;
    case 12:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            if (self) {
                self->GetPosition(selfPos);
            } else if (g_pBSObject && g_pBSObject->trigger && g_pBSObject->trigger->core) {
                TriggerCoreView* core = g_pBSObject->trigger->core;
                selfPos.Set(core->x, core->y, core->z);
            }
            node->GetPosition(targetPos);
            isFloat = true;
            float dx = selfPos.x - targetPos.x;
            float dy = selfPos.y - targetPos.y;
            fresult = dx * dx + dy * dy;
        }
        break;
    case 13:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            if (self) {
                self->GetPosition(selfPos);
            } else if (g_pBSObject && g_pBSObject->trigger && g_pBSObject->trigger->core) {
                TriggerCoreView* core = g_pBSObject->trigger->core;
                selfPos.Set(core->x, core->y, core->z);
            }
            node->GetPosition(targetPos);
            isFloat = true;
            float dx = selfPos.x - targetPos.x;
            float dy = selfPos.y - targetPos.y;
            float dz = selfPos.z - targetPos.z;
            fresult = dx * dx + dy * dy + dz * dz;
        }
        break;
    case 14:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            CVector3 right;
            self->GetPosition(selfPos);
            node->GetPosition(targetPos);
            self->GetRightward(right);
            fresult = MathFunGetPanAngleDiffNoRoll(&selfPos, &targetPos, &right);
            isFloat = true;
        }
        break;
    case 15:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            CVector3 right;
            CVector3 forward;
            self->GetPosition(selfPos);
            node->GetPosition(targetPos);
            self->GetRightward(right);
            self->GetForward(forward);
            fresult = MathFunGetTiltAngleDiffNoRoll(&selfPos, &targetPos, &right, &forward);
            isFloat = true;
        }
        break;
    case 16:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            self->GetPosition(selfPos);
            node->GetPosition(targetPos);
            isFloat = true;
            fresult = targetPos.z - selfPos.z;
        }
        break;
    case 17: {
        CVector3 right;
        CAIFilterRealPosition guess;
        self->GetAIDoodad()->filter->GetTargetGuessPosition(guess);
        self->GetPosition(selfPos);
        guess -= selfPos;
        self->GetRightward(right);
        float facing = MathFunAtan2F(-right.y, right.x);
        fresult = MathFunNormalizeAngleNegativePiToPi(MathFunAtan2F(guess.x, guess.y) - facing);
        isFloat = true;
        break;
    }
    case 18: {
        CVector3 right;
        CVector3 forward;
        CAIFilterRealPosition guess;
        self->GetAIDoodad()->filter->GetTargetGuessPosition(guess);
        self->GetPosition(selfPos);
        self->GetRightward(right);
        float facing = MathFunAtan2F(-right.y, right.x);
        self->GetForward(forward);
        MathFunRotateAboutZ(&forward.x, &forward.y, -facing);
        guess -= selfPos;
        MathFunRotateAboutZ(&guess.x, &guess.y, -MathFunAtan2F(guess.x, guess.y));
        float pitch = MathFunAtan2F(-forward.z, forward.y);
        fresult = MathFunNormalizeAngleNegativePiToPi(MathFunAtan2F(-guess.z, guess.y) - pitch);
        isFloat = true;
        break;
    }
    case 20: {
        CVector3 right;
        ScriptDataView* data = self->GetScriptObject()->user->GetScriptData();
        self->GetPosition(selfPos);
        targetPos.x = data->x;
        targetPos.y = data->y;
        targetPos.z = data->z;
        targetPos -= selfPos;
        self->GetRightward(right);
        float facing = MathFunAtan2F(-right.y, right.x);
        fresult = MathFunNormalizeAngleNegativePiToPi(MathFunAtan2F(targetPos.x, targetPos.y) - facing);
        isFloat = true;
        break;
    }
    case 21: {
        CVector3 right;
        if (self)
            filter = self->GetAIDoodad()->filter;
        CAIObject* ai = filter->m_ai;
        { float z = ai->position.z; float y = ai->position.y; float x = ai->position.x; pos.Set(x, y, z); }
        self->GetPosition(selfPos);
        pos -= selfPos;
        self->GetRightward(right);
        float facing = MathFunAtan2F(-right.y, right.x);
        fresult = MathFunNormalizeAngleNegativePiToPi(MathFunAtan2F(pos.x, pos.y) - facing);
        isFloat = true;
        break;
    }
    case 23:
        iresult = 0;
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            CAIFilterRealPosition start;
            self->GetPosition(targetPos);
            start = targetPos;
            node->GetPosition(targetPos);
            pos = targetPos;
            start.z += value;
            pos.z += 0.8f;
            iresult = !CheckVisionLine(start, pos, 0);
        }
        break;
    case 24:
        isFloat = true;
        fresult = 0.0f;
        if (self && self->AsSoldierObject()) {
            CAIFilterObject* f = GetFilter(self);
            if (f && f->m_ai->lookPointSet) {
                if (target)
                    basic = target->user;
                if (basic)
                    node = basic->GetSceneNode();
                if (node) {
                    CAIObject* ai = f->m_ai;
                    { float z = ai->lookPoint.z; float y = ai->lookPoint.y; float x = ai->lookPoint.x; pos.Set(x, y, z); }
                    selfPos = pos;
                    node->GetPosition(targetPos);
                    selfPos -= targetPos;
                    fresult = sqrtf(selfPos.x * selfPos.x + selfPos.y * selfPos.y + selfPos.z * selfPos.z);
                }
            }
        }
        break;
    case 25:
        iresult = 0;
        if (self->AsSoldierObject()) {
            CAIObject* ai = GetFilter(self)->m_ai;
            if (ai->lookPointSet) {
                { float z = ai->lookPoint.z; float y = ai->lookPoint.y; float x = ai->lookPoint.x; pos.Set(x, y, z); }
                iresult = ai->IsObjectInLineOfSight(pos, value, false);
            }
        }
        break;
    case 26:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            CAIFilterObject* f = GetFilter(self);
            node->GetPosition(targetPos);
            pos = targetPos;
            iresult = f->m_ai->IsObjectInLineOfSight(pos, value, false);
        }
        break;
    case 27:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            CAIFilterObject* f = GetFilter(self);
            node->GetPosition(targetPos);
            pos = targetPos;
            iresult = f->m_ai->IsObjectInFieldOfView(pos, value, 50.0f, false);
        }
        break;
    case 28:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            CAIFilterObject* f = GetFilter(self);
            node->GetPosition(targetPos);
            pos = targetPos;
            iresult = f->m_ai->IsObjectInFieldOfView(pos, value, 50.0f, false);
        }
        break;
    case 29:
        if (self)
            filter = self->GetAIDoodad()->filter;
        (*(ISceneNode**)ARG(2))->GetPosition(targetPos);
        pos = targetPos;
        iresult = filter->m_ai->IsObjectInFieldOfView(pos, value, 50.0f, false);
        break;
    case 30:
        if (self)
            filter = self->GetAIDoodad()->filter;
        (*(ISceneNode**)ARG(2))->GetPosition(targetPos);
        pos = targetPos;
        iresult = filter->m_ai->IsObjectReachable(pos);
        break;
    case 35:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            CVector3 other;
            CVector3 forward;
            if (self) {
                self->GetPosition(selfPos);
            } else if (g_pBSObject && g_pBSObject->trigger && g_pBSObject->trigger->core) {
                TriggerCoreView* core = g_pBSObject->trigger->core;
                selfPos.Set(core->x, core->y, core->z);
            }
            node->GetForward(forward);
            node->GetPosition(other);
            CVector3 d;
            d.x = selfPos.x - other.x;
            d.y = selfPos.y - other.y;
            d.z = selfPos.z - other.z;
            float length = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
            if (length != 0.0f) {
                float scale = 1.0f / length;
                d.x *= scale;
                d.y *= scale;
            }
            isFloat = true;
            float dot = d.x;
            dot *= forward.x;
            dot += d.y * forward.y;
            fresult = dot;
        } else {
            isFloat = true;
            fresult = 0.0f;
        }
        break;
    case 37:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            CVector3 forward;
            CVector3 other;
            self->GetForward(forward);
            self->GetPosition(selfPos);
            node->GetPosition(other);
            float dot = other.x - selfPos.x;
            dot *= forward.x;
            float dy = other.y - selfPos.y;
            dot += dy * forward.y;
            iresult = dot < 0.0f;
        }
        break;
    case 38:
        if (target)
            basic = target->user;
        if (basic)
            node = basic->GetSceneNode();
        if (node) {
            CVector3 other;
            CVector3 right;
            node->GetRightward(right);
            self->GetPosition(selfPos);
            node->GetPosition(other);
            CVector3 d;
            d.x = selfPos.x - other.x;
            d.y = selfPos.y - other.y;
            d.z = selfPos.z - other.z;
            float length = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
            if (length != 0.0f) {
                float scale = 1.0f / length;
                d.x *= scale;
                d.y *= scale;
            }
            isFloat = true;
            float dot = d.x;
            dot *= right.x;
            dot += d.y * right.y;
            fresult = dot;
        } else {
            isFloat = true;
            fresult = 0.0f;
        }
        break;
    case 44: {
        BSObjectView* script = g_scene.GetPlayer(0)->GetScriptObject();
        ScriptDataView* data;
        if (script && (data = script->user->GetScriptData()))
            iresult = data->value;
        break;
    }
    case 45: {
        BSObjectView* script = g_scene.GetPlayer(0)->GetScriptObject();
        TriggerCoreView* core;
        if (script && (core = script->trigger->core))
            iresult = core->health;
        break;
    }
    case 60:
        if (target) {
            if (g_bInMultiplayerMode) {
                if (target && target->user && target->user->GetSceneNode() &&
                    target->user->GetSceneNode()->AsPlayerObject())
                    iresult = 1;
                else
                    iresult = 0;
            } else {
                iresult = target == g_scene.GetPlayer(0)->GetScriptObject();
            }
        }
        break;
    case 61:
        if (target) {
            if (target == g_scene.GetPlayer(0)->GetScriptObject())
                iresult = 1;
            else
                iresult = target->trigger->core->team == 1;
        }
        break;
    case 62:
        if (target) {
            if (target == g_scene.GetPlayer(0)->GetScriptObject())
                iresult = 0;
            else
                iresult = target->trigger->core->team == 2;
        }
        break;
    case 63:
        if (target) {
            if (target == g_scene.GetPlayer(0)->GetScriptObject())
                iresult = 0;
            else
                iresult = target->trigger->core->team == 0;
        }
        break;
    case 64:
        if (target)
            iresult = target->trigger->core->field38;
        break;
    case 65:
        if (target) {
            if (target == g_scene.GetPlayer(0)->GetScriptObject())
                iresult = 1;
            else
                iresult = target->trigger->core->team;
        }
        break;
    case 66:
        if (target)
            iresult = target->trigger->core->flags & 1;
        break;
    case 67:
        if (target)
            iresult = target->trigger->core->field3a;
        break;
    case 68:
        if (target)
            iresult = (target->trigger->core->flags >> 1) & 1;
        break;
    case 69:
        if (target)
            iresult = target->trigger->core->field3c;
        break;
    case 70:
        if (target)
            iresult = target->trigger->core->health;
        break;
    case 71:
        if (target)
            iresult = target->trigger->core->health;
        break;
    case 72:
        if (target && target->user)
            iresult = (int)target->user->GetSceneNode();
        break;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    if (isFloat)
        **stack = *(int*)&fresult;
    else
        **stack = *(int*)&iresult;
}
