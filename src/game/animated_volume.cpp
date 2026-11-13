// animated_volume.cpp (0x800bfa2c): CAnimatedVolume, a bounding sphere with
// up to 24 per-bone boxes of an animated object. TransformedCopy (the
// sphere's copy, then the members, with the boxes marked stale), Create (a new
// volume from DWI_alloc through the inline constructors), the double-dispatch
// collision tests (swapped tests reverse the collision order and let the other
// volume test this one; fiber tests set the collision's line flag and test the
// fiber's line; the rest are the sphere's), the line test (first against the
// bounding sphere on a copy of the collision, a miss raising the result to 1;
// otherwise the per-bone boxes are rebuilt once - the object's matrix from its
// virtual slot at +60, each entry's bone matrix converted with TibToMOHFL, the
// entry's box transformed by it and then by the object matrix - every box is
// tested against the line into its own collision record, and of the hits the
// enabled map entry with the highest priority (ties: the smaller time) wins;
// its time, line flag and result are copied, and a contact (2) adds its
// contact with the map entry's value as its index), Set (keeps the
// location-collision data and map, boxes stale), the destructor and the
// constructor. The file owns __vt__15CAnimatedVolume and the RTTI records of
// IVolume, CVolSphere and CAnimatedVolume. The classes come from the mangled
// symbols; IVolume's virtual functions are declared in the order of
// __vt__7IVolume (CAnimatedVolume adds the CCSGVolume test); the members, the
// collision and contact views, the location-collision data and map layouts,
// the object view (placeholder virtuals before slot +60, bones at +3904), the
// inline helpers, the inline class operator new (DWI_alloc with no name and
// flag 1024) and the result codes are inferred. The volume pointer assigned
// inside the sphere call is what keeps 'this' in the original's register.
// Compiled with deferred inlining (Create inlines the constructor defined at
// the end), so the functions are written in reverse image order.
#include <math.h>
#include "game/CVector3.h"
#include "game/CMatrix.h"

void* DWI_alloc(const char*, int, int);

class CDrawContext;
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
class CCSGVolume;
class IVolume;

struct SClsnContact {
    CVector3Data m_vector0;
    CVector3Data m_vector1;
    float m_separation;
    int m_index;
    const IVolume* m_object;
} __attribute__((aligned(16)));

class CCollision {
public:
    CCollision();
    CCollision(const CCollision&);
    ~CCollision();
    CCollision& operator=(const CCollision&);
    void SwapOrder();
    void AddContact(const SClsnContact&);
    void AddContact(const SClsnContact& source, int index) {
        SClsnContact contact;
        contact.m_separation = source.m_separation;
        contact.m_vector0 = source.m_vector0;
        contact.m_vector1 = source.m_vector1;
        contact.m_index = index;
        AddContact(contact);
    }

    unsigned char unknown00[12];
    float m_time;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
    SClsnContact* m_contact;
    unsigned char unknown1c[4];
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
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
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CCSGVolume : public IVolume {
public:
    virtual ~CCSGVolume();
};

class CCDBObject : public IVolume {
public:
    virtual ~CCDBObject();
};

class CWorldVolume : public IVolume {
public:
    virtual ~CWorldVolume();
};

class CVolFiber : public IVolume {
public:
    virtual ~CVolFiber();
    const CLine3& GetLine() const;
};

class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual ~CVolSphere();
    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
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

    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown04[28];
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);

    unsigned char unknown04[76];
} __attribute__((aligned(8))); /* it holds 8-aligned vectors */

// Inferred: one location-collision entry (a box descriptor of 128 bytes
// with its bone index at +112) and the entry count at +3072.
struct CObjLocationCollisionEntry {
    unsigned char unknown00[112];
    int m_bone;
    unsigned char unknown74[12];
};

struct CObjLocationCollisionData {
    CObjLocationCollisionEntry m_entries[24];
    int m_count;
};

struct CObjLocationCollisionMapEntry {
    int m_value;
    int m_priority;
    bool m_enabled;
};

struct CObjLocationCollisionMap {
    CObjLocationCollisionMapEntry m_entries[24];
};

// Inferred: the animated object view (the matrix getter at slot +60).
class CAnimObjectView {
public:
    virtual void Slot08();
    virtual void Slot0c();
    virtual void Slot10();
    virtual void Slot14();
    virtual void Slot18();
    virtual void Slot1c();
    virtual void Slot20();
    virtual void Slot24();
    virtual void Slot28();
    virtual void Slot2c();
    virtual void Slot30();
    virtual void Slot34();
    virtual void Slot38();
    virtual void GetTM(CMatrix&);

    unsigned char unknown04[3900];
    CMatrix m_bones[1];
};

class CAnimatedVolume : public CVolSphere {
public:
    CAnimatedVolume();
    virtual ~CAnimatedVolume();
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
    virtual int TestCollision(const CCSGVolume&, CCollision&, bool) const;
    void Set(CObjLocationCollisionData*, CObjLocationCollisionMap*);
    void UpdateBox(int i, CMatrix* bones, const CMatrix& tm) {
        CMatrix bone(bones[m_data->m_entries[i].m_bone]);
        bone.TibToMOHFL();
        CVolBox box;
        box.TransformedCopy((const IVolume&)m_data->m_entries[i], bone);
        m_boxes[i].TransformedCopy(box, tm);
    }
    void UpdateBoxes() {
        if (!m_valid) {
            CMatrix tm;
            m_object->GetTM(tm);
            CMatrix* bones = m_object->m_bones;
            if (m_data) {
                for (int i = 0; i < m_data->m_count; i++)
                    UpdateBox(i, bones, tm);
            }
            m_valid = true;
        }
    }

    bool m_valid;
    CObjLocationCollisionData* m_data;
    CObjLocationCollisionMap* m_map;
    CVolBox m_boxes[24];
    CAnimObjectView* m_object;
    unsigned char unknown7b4[12];
};

CAnimatedVolume::CAnimatedVolume() {
    m_data = 0;
    m_valid = false;
    m_map = 0;
    m_object = 0;
}

CAnimatedVolume::~CAnimatedVolume() {
}

void CAnimatedVolume::Set(CObjLocationCollisionData* data, CObjLocationCollisionMap* map) {
    m_data = data;
    m_map = map;
    m_valid = false;
}

int CAnimatedVolume::TestCollision(const CLine3& line, CCollision& collision, bool flag) const {
    int count;
    const CAnimatedVolume* volume;
    CCollision sphereCollision(collision);
    if ((volume = this)->CVolSphere::TestCollision(line, sphereCollision, false) == 1) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    CCollision collisions[24];
    int indices[24];
    float times[24];
    count = 0;
    ((CAnimatedVolume*)volume)->UpdateBoxes();
    for (int i = 0; i < volume->m_data->m_count; i++) {
        collisions[i] = collision;
        if (volume->m_boxes[i].TestCollision(line, collisions[i], true) != 1) {
            indices[count] = i;
            times[count] = collisions[i].m_time;
            count++;
        }
    }
    int index;
    int best = -1;
    int priority = 0;
    int value = 0;
    float bestTime = 1e30f;
    for (int j = 0; j < count; j++) {
        index = indices[j];
        CObjLocationCollisionMapEntry* entry = &volume->m_map->m_entries[index];
        if (entry->m_enabled) {
            bool better = false;
            if (entry->m_priority > priority)
                better = true;
            else if (entry->m_priority == priority && (float)fabs(times[j]) < bestTime)
                better = true;
            if (better) {
                value = entry->m_value;
                best = index;
                bestTime = times[j];
                priority = entry->m_priority;
            }
        }
    }
    int result;
    if (best == -1) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        result = 1;
    } else {
        CCollision& hit = collisions[best];
        collision.m_time = hit.m_time;
        collision.m_line = hit.m_line;
        int code;
        result = code = hit.m_result;
        if (flag) {
            if (code > collision.m_result)
                collision.m_result = code;
            if (result == 2)
                collision.AddContact(*hit.m_contact, value);
        }
    }
    return result;
}

int CAnimatedVolume::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CAnimatedVolume::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

int CAnimatedVolume::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

int CAnimatedVolume::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

int CAnimatedVolume::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CAnimatedVolume::TestCollision(CVector3 point, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(point, collision, flag);
}

int CAnimatedVolume::TestCollision(const CPlane& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

int CAnimatedVolume::TestCollision(const CTriangle& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

int CAnimatedVolume::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    collision.m_line = 1;
    return TestCollision(other.GetLine(), collision, flag);
}

int CAnimatedVolume::TestCollision(const CWorldVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CAnimatedVolume::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CAnimatedVolume::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision((const CVolSphere&)other, collision, flag);
}

IVolume* CAnimatedVolume::Create() const {
    return new CAnimatedVolume;
}

void CAnimatedVolume::TransformedCopy(const IVolume& volume, const CMatrix& matrix) {
    const CAnimatedVolume& other = (const CAnimatedVolume&)volume;

    CVolSphere::TransformedCopy(volume, matrix);
    m_data = other.m_data;
    m_map = other.m_map;
    m_valid = false;
    m_object = other.m_object;
}
