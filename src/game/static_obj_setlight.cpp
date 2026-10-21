// A fragment of static_obj.cpp (0x800aee38): CStaticObject::SetAttachedLight
// swaps the attached light's subscription when the light changes (cancelling
// the old one and subscribing to the new light so the pointer is cleared when
// it is marked for destruction), then, with a light attached, stores the
// offset and places the light at the object's local-to-world matrix
// pre-translated by it. The classes and functions are named by the symbols;
// the members, the light's virtual slot and the CMatrix and CVector3 views
// (as in Soldier_object_look.cpp) are inferred. The rest of the file is in
// other units.
class ISubject;
class IObserver;
enum ESubjectEvent {};

void SECB_NullifyPointerOnMarked(IObserver&, ISubject**, ESubjectEvent);

class CPublisher {
public:
    static void CancelSubscription(IObserver&, ISubject**);
    static void AddSubscription(IObserver&, ISubject**, void (*)(IObserver&, ISubject**, ESubjectEvent));
};

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Pair d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    void PreTranslate(CVector3);
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

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
    virtual int GetLocalBoundingVolume(int) const;
    virtual int GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
};

/* inferred: the light's transform setter (slot +232) */
class CLight {
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
    virtual void vfe0();
    virtual void vfe4();
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class CStaticObject : public ISceneNode {
public:
    void SetAttachedLight(CLight*, CVector3);

    unsigned char unknown004[584];
    CLight* m_light;
    CVector3 m_lightOffset;
};

void CStaticObject::SetAttachedLight(CLight* light, CVector3 offset) {
    if (light != m_light) {
        if (&m_light)
            CPublisher::CancelSubscription(*(IObserver*)this, (ISubject**)&m_light);
        m_light = light;
        if (light)
            CPublisher::AddSubscription(*(IObserver*)this, (ISubject**)&m_light, SECB_NullifyPointerOnMarked);
    }
    if (m_light) {
        m_lightOffset = offset;
        CMatrix matrix;
        GetTMLocalToWorld(matrix);
        matrix.PreTranslate(m_lightOffset);
        m_light->SetTMLocalToWorld(matrix);
    }
}
