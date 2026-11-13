// A fragment of world_volume.cpp (0x800cc594): the weak instances of
// CWorldVolume::TestCollisionWithSubVolumes for animated volumes, fibers,
// lines, capsules, spheres and boxes (in the order the file's tests first use
// them). Each tests every sub-volume of the world volume's list into a copy
// of the collision: with the collision's line flag set, the nearest contact
// (smallest depth) that is not a miss is kept; otherwise the highest result
// is kept and a result of 3 ends the search. The template, the classes and
// the list's template are named by the mangled symbols; the template is
// defined __declspec(weak) out of the class and instantiated explicitly. The
// list (as in world_volume_list.cpp, at +4 after the virtual table pointer),
// the collision record view (32 bytes, out-of-line copy, assignment and
// destructor; depth at +12) and the int-sized results are inferred. The rest
// of the file is not part of this unit.
class CMatrix;
class CVector3;
class CTriangle;
class CPlane;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

class CCollision {
public:
    CCollision(const CCollision&);
    ~CCollision();
    CCollision& operator=(const CCollision&);

    unsigned char unknown00[12];
    float m_depth;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[15];
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
};

class CCDBObject : public IVolume {};

#include "game/DwiContainers.h"

class CWorldVolume : public IVolume {
public:
    template <class T> int TestCollisionWithSubVolumes(const T&, CCollision&, bool) const;

    dwi::list<const CCDBObject*> m_subVolumes;
};

template <class T>
__declspec(weak) int CWorldVolume::TestCollisionWithSubVolumes(const T& other, CCollision& collision, bool flag) const {
    const dwi::list_node<const CCDBObject*>* node = m_subVolumes.m_head.m_next;
    const dwi::list_node<const CCDBObject*>* end = &m_subVolumes.m_head;
    int result = 1;
    for (; node != end; node = node->m_next) {
        const CCDBObject* volume = node->m_data;
        CCollision local(collision);
        int test = volume->TestCollision(other, local, flag);
        if (collision.m_line) {
            if (test != 1 && local.m_depth <= collision.m_depth) {
                collision = local;
                result = test;
            }
        } else {
            if (test >= result) {
                result = test;
                collision = local;
            }
            if (result == 3)
                break;
        }
    }
    return result;
}

template int CWorldVolume::TestCollisionWithSubVolumes<CAnimatedVolume>(const CAnimatedVolume&, CCollision&, bool) const;
template int CWorldVolume::TestCollisionWithSubVolumes<CVolFiber>(const CVolFiber&, CCollision&, bool) const;
template int CWorldVolume::TestCollisionWithSubVolumes<CLine3>(const CLine3&, CCollision&, bool) const;
template int CWorldVolume::TestCollisionWithSubVolumes<CVolCapsule>(const CVolCapsule&, CCollision&, bool) const;
template int CWorldVolume::TestCollisionWithSubVolumes<CVolSphere>(const CVolSphere&, CCollision&, bool) const;
template int CWorldVolume::TestCollisionWithSubVolumes<CVolBox>(const CVolBox&, CCollision&, bool) const;
