// A fragment of attachobject.cpp (0x800972ac): CAttachableObject's
// CommitUpdate (CStaticObject's, then UpdateAttachedObjects inlined),
// AttemptUpdate and BeginUpdate (CStaticObject's), the empty CommitAI and
// UpdateAI, UpdateAttachedObjects (each occupied slot through the per-slot
// inline described in attachobject_ctor.cpp; the views are the same,
// inferred), IsVisible and Draw (CStaticObject's) and DetachObject (every
// slot holding the object is cleared, an animated object told it is detached,
// a player's two flag bits reset through inferred inline setters and its
// weapon shown; true when one was found). This fragment is compiled with
// -inline deferred,auto, which is why CommitUpdate can inline the later
// UpdateAttachedObjects: the functions are written in reverse image order.
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

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix& operator=(const CMatrix&);
    void Multiply(const CMatrix&, const CMatrix&);
    void Inverse(const CMatrix&);
    CVector3 GetPos() const { return *(const CVector3*)m[3]; }
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

struct ScriptObjectView {
    unsigned char unknown00[16];
    int id;
};

class CStaticObject;
class CAnimObject;
class CPlayerObject;

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
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
    virtual ScriptObjectView* GetScriptObject() const;
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual CStaticObject* AsStaticObject();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual CAnimObject* AsAnimObject();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual CPlayerObject* AsPlayerObject();
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
    virtual void unknown0e4();
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class CDrawContext;

class CStaticObject : public ISceneNode {
public:
    bool IsVisible(CDrawContext&) const; /* result type inferred */
    void Draw(CDrawContext&);
    void CommitUpdate();
    void AttemptUpdate(float); /* result type not known */
    void BeginUpdate(float);

    unsigned char data004[60];
    CMatrix m_tm;
    unsigned char data080[512];
};

class CAnimObject : public ISceneNode {
public:
    void SetAttached(bool);

    unsigned char data004[3900];
    CMatrix m_unknownF40;
    unsigned char data0f80[5428];
    bool m_unknown24b4;
};

class CPlayerObject : public ISceneNode {
public:
    void SetHideWeapon(bool);
    /* Inferred inline setters (names not original). */
    void SetFlag02(bool on) { m_flag02 = on; }
    void SetFlag01(bool on) { m_flag01 = on; }

    unsigned char data004[914];
    bool m_flag80 : 1;
    bool m_flag40 : 1;
    bool m_flag20 : 1;
    bool m_flag10 : 1;
    bool m_flag08 : 1;
    bool m_flag04 : 1;
    bool m_flag02 : 1;
    bool m_flag01 : 1;
    unsigned char data397[937];
    CVector3 m_position;
};

class CAttachableObject : public CStaticObject {
public:
    virtual void MarkForDestruction(int); /* defined elsewhere; keeps the table out */
    void CommitUpdate();
    void AttemptUpdate(float); /* result type not known */
    void CommitAI();
    void UpdateAI(float);
    void BeginUpdate(float);
    void UpdateAttachedObjects();
    bool IsVisible(CDrawContext&) const; /* result type inferred */
    void Draw(CDrawContext&);
    bool DetachObject(ISceneNode*); /* result type inferred */
    void UpdateAttachedObject(int);

    ISceneNode* m_attached[10];
    unsigned char unknown2a8[8];
    CMatrix m_attachMatrix[10];
    int m_attachFlag[10];
    int m_attachScriptId[10];
};

inline void CAttachableObject::UpdateAttachedObject(int i) {
    CMatrix tm;
    if (m_attached[i]->GetScriptObject()->id != m_attachScriptId[i]) {
        m_attached[i] = 0;
    } else {
        tm = m_tm;
        tm.Multiply(m_attachMatrix[i], tm);
        switch (m_attachFlag[i]) {
        case 1:
            m_attached[i]->AsStaticObject()->SetTMLocalToWorld(tm);
            break;
        case 2: {
            CAnimObject* anim = m_attached[i]->AsAnimObject();
            if (anim->m_unknown24b4) {
                CMatrix inverse;
                inverse.Inverse(anim->m_unknownF40);
                tm.Multiply(inverse, tm);
            }
            anim->SetTMLocalToWorld(tm);
            break;
        }
        case 3:
            if (m_attached[i]->AsPlayerObject()->m_flag01)
                m_attached[i]->AsPlayerObject()->m_position = tm.GetPos();
            else
                m_attached[i]->AsPlayerObject()->SetTMLocalToWorld(tm);
            break;
        }
    }
}

bool CAttachableObject::DetachObject(ISceneNode* object) {
    bool found = false;
    for (int i = 0; i < 10; i++) {
        if (m_attached[i] == object) {
            found = true;
            if (m_attachFlag[i] == 2) {
                m_attached[i]->AsAnimObject()->SetAttached(false);
            } else if (m_attachFlag[i] == 3) {
                m_attached[i]->AsPlayerObject()->SetFlag02(false);
                m_attached[i]->AsPlayerObject()->SetFlag01(true);
                m_attached[i]->AsPlayerObject()->SetHideWeapon(false);
            }
            m_attached[i] = 0;
            m_attachFlag[i] = 0;
        }
    }
    return found;
}

void CAttachableObject::Draw(CDrawContext& context) {
    CStaticObject::Draw(context);
}

bool CAttachableObject::IsVisible(CDrawContext& context) const {
    return CStaticObject::IsVisible(context);
}

void CAttachableObject::UpdateAttachedObjects() {
    for (int i = 0; i < 10; i++) {
        if (m_attached[i])
            UpdateAttachedObject(i);
    }
}

void CAttachableObject::BeginUpdate(float time) {
    CStaticObject::BeginUpdate(time);
}

void CAttachableObject::UpdateAI(float) {
}

void CAttachableObject::CommitAI() {
}

void CAttachableObject::AttemptUpdate(float time) {
    CStaticObject::AttemptUpdate(time);
}

void CAttachableObject::CommitUpdate() {
    CStaticObject::CommitUpdate();
    UpdateAttachedObjects();
}
