// A fragment of static_obj.cpp (0x800afbf8): CStaticObject's GetCollisionId (the id at +476) and IsVisible (always
// true; the member is inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
enum EClsnId {};
class CDrawContext;

class CStaticObject {
public:
    EClsnId GetCollisionId() const;
    int IsVisible(CDrawContext&) const;

    unsigned char unknown000[476];
    EClsnId m_collisionId;
};

EClsnId CStaticObject::GetCollisionId() const {
    return m_collisionId;
}

int CStaticObject::IsVisible(CDrawContext&) const {
    return 1;
}
