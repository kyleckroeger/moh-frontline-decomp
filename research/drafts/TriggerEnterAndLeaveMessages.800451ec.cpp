// A fragment of proxtrig.cpp (0x80044d3c): IsPointInTrigger tests a point
// against a trigger's shape, chosen by the type at +44 of its core: 0 a
// sphere (radius), 1 an upright cylinder (radius, top and bottom heights), 2
// a box oriented by the core's rotation quaternion (the point is taken into
// the box frame through the matrix rows and compared with the half sizes;
// the third axis reuses the second half size as its upper bound, as in the
// original); other types are never inside. Sizes are 12.4 fixed-point shorts
// (+46, +48, +50). The function and the classes are named by the symbols; the
// members, the transform and setter helpers, the row accessor and the
// CVector3 copy forms (a member-initialiser copy constructor and an
// assignment returning by value) are inferred. TriggerActivateMessages (at
// 0x800450c8) sends, for every trigger of the object's proximity leaf whose
// bit is set in the proximity mask (and whose index is not flagged), the
// trigger's activate or deactivate message list (a count followed by the
// messages) to the trigger's script object, from the object's own script
// object; the per-trigger body is an inferred inline helper whose message
// list and count default to empty. BSGO_Basic, BSSendMessage and
// g_pTriggerObjects are named by the symbols; the virtual order follows the
// other BSGO_Basic views (GetProximityData at +0x14); the proximity, leaf and
// message layouts are inferred. The rest of the file is in other units.
union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.c.x = x;
        d.c.y = y;
        d.c.z = z;
    }
    CVector3(const CVector3& o) : d(o.d) {}
    CVector3 operator=(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
        return *this;
    }
    float Dot(float x, float y, float z) const { return d.c.x * x + d.c.y * y + d.c.z * z; }

    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);
    CVector3 GetRow(int i) const { return *(const CVector3*)m[i]; }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

struct TriggerMessagesView {
    unsigned int count;
    unsigned int messages[1];
};

struct MOH_core_Struct {
    int id;
    unsigned char unknown04[8];
    int enabled;
    float x;
    float y;
    float z;
    CQuaternion rotation;
    short type;
    short size0;
    short size1;
    short size2;
    short acquire[4];
    unsigned char unknown3c[12];
    unsigned int kind;
    unsigned char unknown4c[12];
    TriggerMessagesView* enter;
    TriggerMessagesView* leave;
    TriggerMessagesView* enterActivate;
    TriggerMessagesView* leaveActivate;
    TriggerMessagesView* deactivate;
    TriggerMessagesView* activate;
};

struct BSObject;

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    BSObject* bsObject;
};

static inline void Set3(float& a, float& b, float& c, float x, float y, float z) { a = x; b = y; c = z; }

static inline void Dots(float& d0, float& d1, float& d2, const CVector3& r0, const CVector3& r1, const CVector3& r2, float dx, float dy, float dz) {
    Set3(d0, d1, d2, r0.Dot(dx, dy, dz), r1.Dot(dx, dy, dz), r2.Dot(dx, dy, dz));
}

static inline void ToLocal(const CMatrix& m, const CVector3& point, float& d0, float& d1, float& d2) {
    CVector3 pos;
    pos = m.GetRow(3);
    float dy = point.d.c.y - pos.d.c.y;
    float dx = point.d.c.x - pos.d.c.x;
    float dz = point.d.c.z - pos.d.c.z;
    Dots(d0, d1, d2, m.GetRow(0), m.GetRow(1), m.GetRow(2), dx, dy, dz);
}

bool IsPointInTrigger(const CVector3& point, const TriggerObject_struct* trigger) {
    MOH_core_Struct* core = trigger->core;
    CVector3 center(core->x, core->y, core->z);
    bool result;
    float dy;
    float dx;
    float dz;
    float radius;
    float bottom;
    float top;
    switch (core->type) {
    case 0: {
        dx = point.d.c.x - center.d.c.x;
        dy = point.d.c.y - center.d.c.y;
        dz = point.d.c.z - center.d.c.z;
        radius = core->size0 / 16.0f;
        if (dx * dx + dy * dy + dz * dz > radius * radius)
            result = false;
        else
            result = true;
        break;
    }
    case 1: {
        radius = core->size0 / 16.0f;
        dy = point.d.c.y - center.d.c.y;
        dx = point.d.c.x - center.d.c.x;
        bottom = core->size2 / 16.0f;
        dz = point.d.c.z - center.d.c.z;
        top = core->size1 / 16.0f;
        if (dx * dx + dy * dy > radius * radius)
            result = false;
        else if (dz <= top && dz >= bottom)
            result = true;
        else
            result = false;
        break;
    }
    case 2: {
        CMatrix matrix;
        trigger->core->rotation.GetMatrix(matrix);
        matrix.SetPos(CVector3(trigger->core->x, trigger->core->y, trigger->core->z));
        core = trigger->core;
        float hx;
        float hy;
        float hz;
        Set3(hz, hy, hx, core->size1 / 16.0f / 2.0f, -(core->size2 / 16.0f) / 2.0f, core->size0 / 16.0f / 2.0f);
        float d0;
        float d1;
        float d2;
        ToLocal(matrix, point, d0, d1, d2);
        if (d0 > hx || d0 < -hx)
            result = false;
        else if (d1 > hy || d1 < -hy)
            result = false;
        else if (d2 <= hy && d2 >= -hz)
            result = true;
        else
            result = false;
        break;
    }
    default:
        result = false;
        break;
    }
    return result;
}

class IMovingSceneNode;
class ISceneNode {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual IMovingSceneNode* AsMovingNode();
};
class BPDLightVolume;
class IMovingSceneNode {
public:
    virtual void m00();
    virtual void m01();
    virtual void m02();
    virtual void m03();
    virtual void m04();
    virtual void m05();
    virtual void m06();
    virtual void m07();
    virtual void m08();
    virtual void m09();
    virtual void m10();
    virtual void m11();
    virtual void m12();
    virtual void m13();
    virtual void m14();
    virtual void m15();
    virtual void m16();
    virtual void m17();
    virtual void m18();
    virtual void m19();
    virtual void m20();
    virtual void m21();
    virtual void m22();
    virtual void m23();
    virtual void m24();
    virtual void m25();
    virtual void m26();
    virtual void m27();
    virtual void m28();
    virtual void m29();
    virtual void m30();
    virtual void m31();
    virtual void m32();
    virtual void m33();
    virtual void m34();
    virtual void m35();
    virtual void m36();
    virtual void m37();
    virtual void m38();
    virtual void m39();
    virtual void m40();
    virtual void m41();
    virtual void m42();
    virtual void m43();
    virtual void m44();
    virtual void m45();
    virtual void m46();
    virtual void m47();
    virtual void m48();
    virtual void m49();
    virtual void m50();
    virtual void m51();
    virtual void m52();
    virtual void m53();
    virtual void m54();
    virtual void m55();
    virtual void m56();
    virtual void m57();
    virtual void m58();
    virtual void m59();
    virtual void m60();
    virtual void m61();
    virtual void m62();
    virtual void m63();
    virtual void m64();
    virtual void m65();
    virtual void m66();
    virtual void m67();
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
};

struct _PropBSPLeaf {
    unsigned int count;
    unsigned char unknown04[28];
    unsigned short* triggers;
};

struct ProximityDataView {
    unsigned int prevMask;
    unsigned int mask;
    _PropBSPLeaf* prevLeaf;
    _PropBSPLeaf* leaf;
};

class BSGO_Basic {
public:
    BSObject* object;
    unsigned char unknown04[8];

    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual ProximityDataView* GetProximityData();
};

extern TriggerObject_struct g_pTriggerObjects[2000];

void BSSendMessage(unsigned short, BSObject*, TriggerObject_struct*, BSObject*, void*);

static inline void SendMessages(unsigned short* trigger, bool activate, BSGO_Basic* object, unsigned int* messages = 0, unsigned int count = 0) {
    unsigned int i;
    unsigned int index = *trigger & 0x7FFF;
    if ((*trigger & 0x8000) >> 15)
        return;
    if (!g_pTriggerObjects[index].core->enabled)
        return;
    if (activate && g_pTriggerObjects[index].core->activate) {
        count = g_pTriggerObjects[index].core->activate->count;
        messages = g_pTriggerObjects[index].core->activate->messages;
    } else if (!activate && g_pTriggerObjects[index].core->deactivate) {
        count = g_pTriggerObjects[index].core->deactivate->count;
        messages = g_pTriggerObjects[index].core->deactivate->messages;
    }
    for (i = 0; i < count; i++, messages++) {
        BSSendMessage(*messages, g_pTriggerObjects[index].bsObject, &g_pTriggerObjects[index], object->object, 0);
    }
}

void TriggerActivateMessages(BSGO_Basic* object, bool activate) {
    unsigned short* trigger;
    unsigned short* end;
    ProximityDataView* proximity;
    unsigned int mask;
    unsigned int bit;
    proximity = object->GetProximityData();
    bit = 1;
    mask = proximity->mask;
    end = proximity->leaf->triggers + proximity->leaf->count;
    for (trigger = proximity->leaf->triggers; trigger != end; trigger++, bit <<= 1) {
        if (mask & bit)
            SendMessages(trigger, activate, object);
    }
}

class CPlayerObject {
public:
    void SetClimbingState(bool);
};

class CWorldObject {
public:
    struct SAcquireSet {
        int a, b, c, d;
    };
    void Acquire(const SAcquireSet&, bool);
};

class CScene {
public:
    unsigned char unknown00[24];
    CWorldObject* world;
    unsigned char unknown1c[100];
};

extern CScene g_scene;
extern BPDLightVolume* g_pLightVolumes;

struct LightVolumeView { unsigned char data[48]; };

static inline void EnterTrigger(unsigned short value, bool activate, BSGO_Basic* object, unsigned int* messages = 0, unsigned int count = 0) {
    unsigned int i;
    unsigned int index = value & 0x7FFF;
    TriggerObject_struct* trigger;
    if ((value & 0x8000) >> 15) {
        IMovingSceneNode* node = object->GetSceneNode()->AsMovingNode();
        if (node)
            node->EnterLightVolume((BPDLightVolume*)((LightVolumeView*)g_pLightVolumes + index));
        return;
    }
    trigger = &g_pTriggerObjects[index];
    if (g_pTriggerObjects[index].core->enabled) {
    if (activate && g_pTriggerObjects[index].core->enterActivate) {
        count = g_pTriggerObjects[index].core->enterActivate->count;
        messages = g_pTriggerObjects[index].core->enterActivate->messages;
    } else if (!activate && g_pTriggerObjects[index].core->enter) {
        count = g_pTriggerObjects[index].core->enter->count;
        messages = g_pTriggerObjects[index].core->enter->messages;
    }
    for (i = 0; i < count; i++, messages++) {
        BSSendMessage(*messages, g_pTriggerObjects[index].bsObject, &g_pTriggerObjects[index], object->object, 0);
    }
    if (trigger->core->enabled == 5 && trigger->core->kind == 0x2070A66D && !activate)
        ((CPlayerObject*)object->GetSceneNode())->SetClimbingState(true);
    } else {
        switch (g_pTriggerObjects[index].core->id) {
        case 0x4D078F23:
            if (!activate) {
                int a = g_pTriggerObjects[index].core->acquire[0]; int b = g_pTriggerObjects[index].core->acquire[1];
                CWorldObject::SAcquireSet set = { a, b, g_pTriggerObjects[index].core->acquire[2], g_pTriggerObjects[index].core->acquire[3] };
                g_scene.world->Acquire(set, true);
            }
            break;
        }
    }
}

static inline void LeaveTrigger(unsigned short value, bool activate, BSGO_Basic* object, unsigned int* messages = 0, unsigned int count = 0) {
    unsigned int i;
    unsigned int index = value & 0x7FFF;
    TriggerObject_struct* trigger;
    if ((value & 0x8000) >> 15) {
        IMovingSceneNode* node = object->GetSceneNode()->AsMovingNode();
        if (node)
            node->ExitLightVolume((BPDLightVolume*)((LightVolumeView*)g_pLightVolumes + index));
        return;
    }
    trigger = &g_pTriggerObjects[index];
    if (g_pTriggerObjects[index].core->enabled) {
    if (activate && g_pTriggerObjects[index].core->leaveActivate) {
        count = g_pTriggerObjects[index].core->leaveActivate->count;
        messages = g_pTriggerObjects[index].core->leaveActivate->messages;
    } else if (!activate && g_pTriggerObjects[index].core->leave) {
        count = g_pTriggerObjects[index].core->leave->count;
        messages = g_pTriggerObjects[index].core->leave->messages;
    }
    for (i = 0; i < count; i++, messages++) {
        BSSendMessage(*messages, g_pTriggerObjects[index].bsObject, &g_pTriggerObjects[index], object->object, 0);
    }
    if (trigger->core->enabled == 5 && trigger->core->kind == 0x2070A66D && !activate)
        ((CPlayerObject*)object->GetSceneNode())->SetClimbingState(false);
    }
}

void TriggerEnterAndLeaveMessages(BSGO_Basic* object, bool activate) {
    ProximityDataView* proximity = object->GetProximityData();
    if (proximity->leaf == proximity->prevLeaf || !proximity->prevLeaf) {
        int i;
        int changed = proximity->mask ^ proximity->prevMask;
        int entered = changed & proximity->mask;
        int left = changed & proximity->prevMask;
        for (i = 0; entered; entered >>= 1, i++) {
            if (entered & 1)
                EnterTrigger(proximity->leaf->triggers[i], activate, object);
        }
        for (i = 0; left; left >>= 1, i++) {
            if (left & 1)
                LeaveTrigger(proximity->leaf->triggers[i], activate, object);
        }
    } else {
        unsigned short* newT;
        unsigned short* oldT;
        unsigned short* newEnd;
        unsigned short* oldEnd;
        unsigned int newBit;
        unsigned int oldBit;
        unsigned int newMask;
        unsigned int oldMask;
        newT = proximity->leaf->triggers;
        newEnd = proximity->leaf->triggers + proximity->leaf->count;
        oldT = proximity->prevLeaf->triggers;
        oldEnd = proximity->prevLeaf->triggers + proximity->prevLeaf->count;
        newMask = proximity->mask;
        oldMask = proximity->prevMask;
        newBit = 1;
        oldBit = 1;
        for (;;) {
            if (newT == newEnd || oldT == oldEnd)
                break;
            if (*newT < *oldT) {
                if (newBit & newMask)
                    EnterTrigger(*newT, activate, object);
                newBit <<= 1;
                newT++;
            } else if (*newT > *oldT) {
                if (oldBit & oldMask)
                    LeaveTrigger(*oldT, activate, object);
                oldBit <<= 1;
                oldT++;
            } else {
                unsigned int newIn = newBit & newMask;
                unsigned int oldIn = oldBit & oldMask;
                if (newIn && !oldIn)
                    EnterTrigger(*newT, activate, object);
                else if (oldIn && !newIn)
                    LeaveTrigger(*oldT, activate, object);
                newBit <<= 1;
                oldBit <<= 1;
                newT++;
                oldT++;
            }
        }
        for (; newT != newEnd; newT++, newBit <<= 1) {
            if (newMask & newBit)
                EnterTrigger(*newT, activate, object);
        }
        for (; oldT != oldEnd; oldT++, oldBit <<= 1) {
            if (oldMask & oldBit)
                LeaveTrigger(*oldT, activate, object);
        }
    }
}
