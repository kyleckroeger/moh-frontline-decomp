// A fragment of AIFilter.cpp (0x8005dba8): the weak
// CAIFilterTankObject::GetLookDirection and CAIFilterSoldierObject::
// GetLookDirection, which ask the owning tank or soldier (the filter's owner
// at +4, its scene object at +80) for its look matrix and return its forward
// row and position, and the weak CAIFilterSoldierObject::SetMoveTarget, which
// passes the target position on to the owning animated object as a vector.
// The classes are named by the mangled symbols; the owner, matrix and vector
// views, the position-as-vector reinterpretation and the SetLookVectors
// inline are inferred. The functions are weak copies of header inlines
// emitted in this file, so they are defined __declspec(weak). The rest of
// the file is not part of this unit.
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
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
} __attribute__((aligned(16)));

class CAIFilterRealVector3 {
public:
    CAIFilterRealVector3() {}
    CAIFilterRealVector3(CVector3 v) : x(v.d.v[0]), y(v.d.v[1]), z(v.d.v[2]) {}

    float x;
    float y;
    float z;
};

class CAIFilterRealPosition;

class CTankObject {
public:
    void GetLookMatrix(CMatrix&);
};

class CSoldierObject {
public:
    void GetLookMatrix(CMatrix&);
};

class CAnimObject {
public:
    void SetMoveTarget(const CVector3&);
};

/* inferred: the filter object's owner (the scene object at +80) */
struct AIFilterOwnerView {
    unsigned char unknown00[80];
    void* object;
};

class CAIFilterObject {
public:
    void* vtable;
    AIFilterOwnerView* m_owner;
};

class CAIFilterTankObject : public CAIFilterObject {
public:
    void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);
};

class CAIFilterSoldierObject : public CAIFilterObject {
public:
    void SetMoveTarget(const CAIFilterRealPosition&);
    void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);
};

/* Inferred: copies the look matrix's forward row and position into the two
   results (each through a by-value vector temporary). */
inline void SetLookVectors(const CMatrix& look, CAIFilterRealVector3& forward, CAIFilterRealVector3& up) {
    forward = CAIFilterRealVector3(look.forward);
    up = CAIFilterRealVector3(look.position);
}

__declspec(weak) void CAIFilterTankObject::GetLookDirection(CAIFilterRealVector3& forward, CAIFilterRealVector3& up) {
    AIFilterOwnerView* owner = m_owner;
    CMatrix look;
    ((CTankObject*)owner->object)->GetLookMatrix(look);
    SetLookVectors(look, forward, up);
}

__declspec(weak) void CAIFilterSoldierObject::SetMoveTarget(const CAIFilterRealPosition& target) {
    ((CAnimObject*)m_owner->object)->SetMoveTarget((const CVector3&)target);
}

__declspec(weak) void CAIFilterSoldierObject::GetLookDirection(CAIFilterRealVector3& forward, CAIFilterRealVector3& up) {
    AIFilterOwnerView* owner = m_owner;
    CMatrix look;
    ((CSoldierObject*)owner->object)->GetLookMatrix(look);
    SetLookVectors(look, forward, up);
}
