// A fragment of attachobject.cpp (0x800974f8): CAttachableObject::
// UpdateAttachedObjects, which updates each of the ten occupied attachment
// slots through the per-slot inline described in attachobject_ctor.cpp (the
// views are the same, inferred).
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

class CStaticObject : public ISceneNode {
public:
    unsigned char data004[60];
    CMatrix m_tm;
    unsigned char data080[512];
};

class CAnimObject : public ISceneNode {
public:
    unsigned char data004[3900];
    CMatrix m_unknownF40;
    unsigned char data0f80[5428];
    bool m_unknown24b4;
};

class CPlayerObject : public ISceneNode {
public:
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
    void UpdateAttachedObjects();
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

void CAttachableObject::UpdateAttachedObjects() {
    for (int i = 0; i < 10; i++) {
        if (m_attached[i])
            UpdateAttachedObject(i);
    }
}
