// A fragment of box.cpp (0x800c5458..0x800c555c): the CVolBox destructor (its virtual
// table pointer, then IVolume's through the inline base destructor, and the
// object freed when asked). The file name is this project's; the original
// record is box.cpp, and the destructor follows box_create.cpp. IVolume and
// CVolBox are named by the mangled symbols; only the virtuals the destructor
// needs are declared. CVolBox declares Create (defined elsewhere) before its
// destructor, so its own virtual table, which belongs with the destructor in
// the original file, is not emitted twice: the override keeps IVolume's slot
// order either way. IVolume's destructor is inline (weak in the original), so
// the compiler's copies of it and of IVolume's weak virtual table are weak
// duplicates, linked to the original copies.
// BoxBoxTest (static), a separating-axis step of the box-box test: the overlap along
// one axis is classified against CONTACT_EPSILON (a global .sdata2 constant
// of the original, declared extern: its file is unknown), a separated axis fails, and a shallower overlap records
// its depth, the sign of the projection and the axis. EAxis is named by the
// mangled symbol; its enumerators are unknown. The parameter names and both
// inline helpers (Classify, Sign) are inferred; Sign's pointer casts keep the
// original's store of the result back to its argument's stack slot.
class IVolume {
public:
    virtual ~IVolume() {}
};

class CVolBox : public IVolume {
public:
    virtual IVolume* Create() const;
    virtual ~CVolBox();
};

CVolBox::~CVolBox() {
}

enum EAxis {};

extern const float CONTACT_EPSILON;

inline int Classify(float d) {
    if (d > CONTACT_EPSILON)
        return 1;
    if (d < -CONTACT_EPSILON)
        return 3;
    return 2;
}

inline float Sign(float x) {
    unsigned int r = *(unsigned int*)&x;
    if (r & 0x7f800000) {
        r &= 0x80000000;
        r |= 0x3f800000;
    } else {
        r = 0;
    }
    x = *(float*)&r;
    return x;
}
static bool BoxBoxTest(float extent, float projection, EAxis axis, float& depth, float& sign, EAxis& bestAxis) {
    float d = (float)__fabs(projection) - extent;
    if (Classify(d) != 1) {
        if (d < depth) {
            depth = d;
            sign = Sign(projection);
            bestAxis = axis;
        }
        return true;
    }
    return false;
}
