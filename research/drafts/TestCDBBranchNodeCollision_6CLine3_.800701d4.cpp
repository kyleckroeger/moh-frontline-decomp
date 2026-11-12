// real2 (2026-11-12): 1 real line, unchanged: the split point is mid.Scale(GetDirection(), t); mid.Add(GetStart()) and SetSE uses m_dir.Sub (the original's CVector3 members), replacing an invented scaling constructor, operator* and operator+= (direct field arguments: 590-607).
class CMatrix;
// Inferred: CVector3 as four floats overlaid with two doubles (its copies
// move doubleword pairs).
union CVector3Data {
    double pair[2];
    float v[4];
};

struct CDBVector;
class CVector3 {
public:
    CVector3() {}
    CVector3(const CDBVector& v);
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    void Scale(const CVector3& o, float k) { d.v[0] = k * o.d.v[0]; d.v[1] = k * o.d.v[1]; d.v[2] = k * o.d.v[2]; }
    void Sub(const CVector3& a, const CVector3& b) { d.v[0] = a.d.v[0] - b.d.v[0]; d.v[1] = a.d.v[1] - b.d.v[1]; d.v[2] = a.d.v[2] - b.d.v[2]; }
    void Add(const CVector3& o) { d.v[0] += o.d.v[0]; d.v[1] += o.d.v[1]; d.v[2] += o.d.v[2]; }
    CVector3Data d;
} __attribute__((aligned(8)));



class CTriangle;
class CPlane;
class CVolBox;

class CLine3 {
public:
    CLine3() {}
    ~CLine3() {}
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.Sub(m_end, m_start);
        m_flag38 = 0;
        m_flag39 = 0;
    }
    CLine3& operator=(const CLine3& other) {
        if (&other != this) {
            m_start = other.m_start;
            m_end = other.m_end;
            m_dir = other.m_dir;
            m_value30 = other.m_value30;
            m_value34 = other.m_value34;
            m_flag38 = other.m_flag38;
            m_flag39 = other.m_flag39;
        }
        return *this;
    }
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }
    CVector3 GetDir() const { return m_dir; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

struct CDBVector {
    float x, y, z;
};

inline CVector3::CVector3(const CDBVector& v) {
    d.v[0] = v.x;
    d.v[1] = v.y;
    d.v[2] = v.z;
}

struct CDBPlane {
    CDBVector m_normal;
    float m_d;
};

class CPlane {
public:
    CPlane(const CDBPlane& p) : m_normal(p.m_normal), m_d(p.m_d) {}
    ~CPlane() {}
    float Dist(const CVector3& v) const {
        return m_normal.d.v[0] * v.d.v[0] + m_normal.d.v[1] * v.d.v[1] + m_normal.d.v[2] * v.d.v[2] - m_d;
    }
    CVector3 m_normal;
    float m_d;
} __attribute__((aligned(16)));
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

class IVolume;
class CVolFiber;

// Inferred: the contact's colliding object at +40.
struct SClsnContactView {
    unsigned char unknown00[40];
    const IVolume* m_object;
};

class CCollision {
public:
    void SwapOrder();

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[7];
    SClsnContactView* m_contact;
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

class CCSGVolume : public IVolume {};

class CVolFiber : public IVolume {
public:
    const CLine3& GetLine() const;
};

extern "C" void MEM_free(void*);
extern "C" unsigned int OSGetTick();

namespace dwi {
// Inferred: the vector over caller storage (inline constructor; the
// destructor frees the storage only when the vector owns it).
template <class T>
class fast_vec {
public:
    fast_vec(T* storage, int capacity, int growth = 12)
        : m_data(storage), m_unknown4(0), m_size(0), m_capacity(capacity), m_owned(false), m_growth(growth) {}
    void push_back(T);
    void push(const T& x) { m_data[m_size++] = x; }
    int size() const { return m_size; }
    T& back() { return m_data[m_size - 1]; }
    void pop_back() { m_size--; }
    ~fast_vec() {
        if (m_owned && m_data)
            MEM_free(m_data);
    }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

// Inferred: a tri group's mark at +16, negated after each test.
struct CDBTriGroup {
    unsigned char unknown00[16];
    int m_mark;
};

struct CDBNode {
    int m_type;
};

struct CDBBox;

struct CDBLeafNode : CDBNode {
    unsigned char m_box[60];
    int m_groupCount;
    CDBTriGroup* m_groups[1];
};

struct CDBBranchNode : CDBNode {
    CDBNode* m_front;
    CDBNode* m_back;
    CDBPlane m_plane;
};

struct SLocalFrame {
    SLocalFrame() {}
    ~SLocalFrame() {}

    const CDBBranchNode* m_node;
    CLine3 m_line;
};

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();

    float m_values[3];
    CVector3 m_vectors[4];
};

class CCDBVolBox : public CVolBox {
public:
    CCDBVolBox(const CDBBox&);
    virtual ~CCDBVolBox() {}
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
};

int TestCollisionWithTriGroup(const CDBTriGroup&, const CLine3&, CCollision&, bool);

// Inferred: the collision data's root branch node at +48.
struct CDBDataView {
    unsigned char unknown00[48];
    CDBBranchNode* m_root;
};

// The profiler bins (24 bytes each; time at +8 and count at +20 inferred).
struct SProfileBin {
    SProfileBin();

    int unknown00;
    int unknown04;
    int m_time;
    int unknown0c;
    int unknown10;
    int m_count;
};

extern SProfileBin g_ProfileBins[31];

template <class T>
int TestCollisionWithTriGroups(const dwi::fast_vec<CDBTriGroup*>&, const T&, CCollision&, bool);

class CCDBObject : public IVolume {
public:
    virtual ~CCDBObject();
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
    template <class T>
    int TestCDBLeafNodeCollision(const CDBLeafNode&, const T&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
    template <class T>
    int TestCDBBranchNodeCollision(const CDBBranchNode&, const T&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;

    void GetExtents(CVector3&, CVector3&) const;

    CDBDataView* m_data;
    CVector3 m_min;
    CVector3 m_max;
};


int CCDBObject::TestCollision(const CLine3& other, CCollision& collision, bool flag) const {
    g_ProfileBins[24].m_time -= OSGetTick();
    OSGetTick();
    CDBTriGroup* storage[2048];
    dwi::fast_vec<CDBTriGroup*> groups(storage, 2048);
    int result = TestCDBBranchNodeCollision(*m_data->m_root, other, collision, flag, groups);
    for (CDBTriGroup** group = groups.m_data; group != groups.m_data + groups.m_size; group++)
        (*group)->m_mark = -(*group)->m_mark;
    OSGetTick();
    g_ProfileBins[24].m_time += OSGetTick();
    g_ProfileBins[24].m_count++;
    return result;
}

int CCDBObject::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    g_ProfileBins[28].m_time -= OSGetTick();
    collision.m_line = 1;
    int result = TestCollision(other.GetLine(), collision, flag);
    if (result == 2 && collision.m_contact)
        collision.m_contact->m_object = this;
    g_ProfileBins[28].m_time += OSGetTick();
    g_ProfileBins[28].m_count++;
    return result;
}

void CCDBObject::GetExtents(CVector3& minimum, CVector3& maximum) const {
    minimum = m_min;
    maximum = m_max;
}

template <>
inline int CCDBObject::TestCDBLeafNodeCollision<CLine3>(const CDBLeafNode& leaf, const CLine3& line, CCollision& collision, bool flag,
                                                       dwi::fast_vec<CDBTriGroup*>& groups) const {
    OSGetTick();
    g_ProfileBins[25].m_time -= OSGetTick();
    CCDBVolBox box(*(const CDBBox*)leaf.m_box);
    int result = 1;
    if (box.TestCollision(line, collision, false) != 1) {
        for (int i = 0; i < leaf.m_groupCount; i++) {
            CDBTriGroup* group = leaf.m_groups[i];
            if (group->m_mark > 0) {
                group->m_mark = -group->m_mark;
                groups.push(group);
                int r = TestCollisionWithTriGroup(*group, line, collision, flag);
                if (r > result && (result = r) == 3)
                    break;
            }
        }
    }
    g_ProfileBins[25].m_time += OSGetTick();
    g_ProfileBins[25].m_count++;
    OSGetTick();
    return result;
}

template <>
int CCDBObject::TestCDBBranchNodeCollision<CLine3>(const CDBBranchNode& root, const CLine3& line, CCollision& collision, bool flag,
                                                  dwi::fast_vec<CDBTriGroup*>& groups) const {
    int result = 1;
    OSGetTick();
    SLocalFrame storage[20];
    SLocalFrame* base = storage;
    dwi::fast_vec<SLocalFrame> stack(storage, 20, 11);
    SLocalFrame start;
    start.m_node = &root;
    start.m_line = line;
    stack.push_back(start);
    while (stack.size() != 0) {
        SLocalFrame cur = stack.back();
        stack.pop_back();
        const CDBNode* first = 0;
        const CDBNode* second = 0;
        CPlane plane(cur.m_node->m_plane);
        float d0 = plane.Dist(cur.m_line.GetStart());
        float d1 = plane.Dist(cur.m_line.GetEnd());
        CLine3 front;
        CLine3 back;
        if (d0 >= 0.0f) {
            first = cur.m_node->m_front;
            if (d1 < 0.0f) {
                second = cur.m_node->m_back;
                float t = d0 / (d0 - d1);
                CVector3 mid;
                mid.Scale(cur.m_line.GetDirection(), t);
                mid.Add(cur.m_line.GetStart());
                front.SetSE(cur.m_line.m_start, mid);
                back.SetSE(mid, cur.m_line.m_end);
            } else {
                front.SetSE(cur.m_line.m_start, cur.m_line.m_end);
            }
        } else {
            first = cur.m_node->m_back;
            if (d1 >= 0.0f) {
                second = cur.m_node->m_front;
                float t = -d0 / (d1 - d0);
                CVector3 mid;
                mid.Scale(cur.m_line.GetDirection(), t);
                mid.Add(cur.m_line.GetStart());
                back.SetSE(cur.m_line.m_start, mid);
                front.SetSE(mid, cur.m_line.m_end);
            } else {
                back = cur.m_line;
            }
        }
        if (first) {
            if (first->m_type == 1) {
                SLocalFrame frame;
                if (first == cur.m_node->m_front)
                    frame.m_line = front;
                else
                    frame.m_line = back;
                frame.m_node = (const CDBBranchNode*)first;
                stack.push_back(frame);
            } else {
                int r = TestCDBLeafNodeCollision(*(const CDBLeafNode*)first, line, collision, flag, groups);
                if (r > result) {
                    result = r;
                    if (result == 3)
                        break;
                }
            }
        }
        if (second) {
            if (second->m_type == 1) {
                SLocalFrame frame;
                if (second == cur.m_node->m_front)
                    frame.m_line = front;
                else
                    frame.m_line = back;
                frame.m_node = (const CDBBranchNode*)second;
                stack.push_back(frame);
            } else {
                int r = TestCDBLeafNodeCollision(*(const CDBLeafNode*)second, line, collision, flag, groups);
                if (r > result) {
                    result = r;
                    if (result == 3)
                        break;
                }
            }
        }
    }
    OSGetTick();
    return result;
}
