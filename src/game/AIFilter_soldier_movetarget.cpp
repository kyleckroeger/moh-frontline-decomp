// A fragment of AIFilter.cpp (0x8005dc64): the weak
// CAIFilterSoldierObject::SetMoveTarget, which passes the target position on
// to the owning animated object (the filter's owner at +4, its scene object at
// +80) as a vector. The classes are named by the mangled symbols; the owner
// view and the position-as-vector reinterpretation are inferred. The function
// is a weak copy of a header inline emitted in this file, so it is defined
// __declspec(weak). The rest of the file is not part of this unit.
class CVector3;
class CAIFilterRealPosition;

class CAnimObject {
public:
    void SetMoveTarget(const CVector3&);
};

/* inferred: the filter object's owner (the scene object at +80) */
struct AIFilterOwnerView {
    unsigned char unknown00[80];
    void* object;
};

class CAIFilterSoldierObject {
public:
    void SetMoveTarget(const CAIFilterRealPosition&);

    void* vtable;
    AIFilterOwnerView* m_owner;
};

__declspec(weak) void CAIFilterSoldierObject::SetMoveTarget(const CAIFilterRealPosition& target) {
    ((CAnimObject*)m_owner->object)->SetMoveTarget((const CVector3&)target);
}
