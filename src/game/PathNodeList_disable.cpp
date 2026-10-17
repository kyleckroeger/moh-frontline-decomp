// A fragment of PathNodeList.cpp (0x80054810, the file's first function):
// CPathNodeList::DisableCollidingNodes takes the volume's extents and, for the
// path-node map cells of the minimum and then the maximum corner (the second
// skipped when it holds the same node list), disables every node of the cell
// whose 1.0-radius sphere does not collide with the volume; a cell with a
// disabled node is erased and re-inserted with its marked flag cleared. The
// per-cell test is an inline helper inlined twice, taking the cell by value
// (its name, DisableNodes, is inferred). The classes, the template and the
// methods are named by the mangled symbols; the node, sphere, collision and
// map views, the cell key (as in PathNodeList_closest.cpp), the map's inline
// find and insert and the node's GetPosition are inferred from the code.
// IVolume's constructor and destructor are inline, so the compiler's copy of
// its destructor is a weak duplicate. The rest of the file is in other units.
class CMatrix;
class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[32];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
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
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual IVolume* Create() const;
    virtual ~CVolSphere();
    int TestCollision(const IVolume&, CCollision&, bool) const;
    void SetCenter(CVector3);
    void SetRadius(float);

    unsigned char unknown04[28];
} __attribute__((aligned(16)));

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition(const CAIFilterRealPosition& o) {
        float z_ = o.z;
        float y_ = o.y;
        float x_ = o.x;
        x = x_;
        y = y_;
        z = z_;
    }

    float x;
    float y;
    float z;
};

class CPathNode {
public:
    CAIFilterRealPosition GetPosition() const { return m_position; }

    CAIFilterRealPosition m_position;
    unsigned char unknown0c[5];
    unsigned char m_enabled : 1;
    unsigned char m_flags : 7;
    unsigned char unknown12[6];
};

namespace dwi {
template <class K, class V>
class hash_map {
public:
    struct value_type {
        K first;
        V second;
    };

    class iterator {
    public:
        iterator(value_type* p) : m_p(p) {}
        iterator(const iterator& o) : m_p(o.m_p) {}
        bool operator==(const iterator& other) const { return m_p == other.m_p; }
        bool operator!=(const iterator& other) const { return m_p != other.m_p; }
        value_type* operator->() const { return m_p; }
        value_type* m_p;
    };

    iterator end() { return iterator(m_data + m_size); }

    iterator find(K key) {
        if (m_count == 0)
            return end();
        value_type* p = m_data + (unsigned long)key % m_size;
        while ((int)p->first != -1) {
            if (p->first == key)
                return iterator(p);
            if (++p == m_data + m_size)
                p = m_data;
        }
        return end();
    }

    void insert(K key, V value) {
        value_type* p = m_data + (unsigned long)key % m_size;
        while ((int)p->first != -1) {
            if (++p == m_data + m_size)
                p = m_data;
        }
        p->first = key;
        p->second = value;
        m_count++;
    }

    void erase(iterator it);

    value_type* m_data;
    int m_size;
    int m_count;
};
}

class CPathNodeList {
public:
    struct PathNodes {
        PathNodes() : count(0), nodes(0), marked(true) {}
        bool operator!=(const PathNodes& o) const { return nodes != o.nodes; }
        int count;
        CPathNode** nodes;
        bool marked;
    };

    static unsigned int MakeKey(const CVector3& p) {
        int cy = (int)((p.y + 2.5f) / 5.0f) + 16384;
        int cx = (int)((p.x + 2.5f) / 5.0f) + 16384;
        return ((cy & 0x7fff) << 16) + ((cx & 0x7fff) << 1);
    }

    static bool DisableNodes(PathNodes cell, const IVolume* volume) {
        CVolSphere sphere;
        sphere.SetRadius(1.0f);
        bool changed = false;
        for (int i = 0; i < cell.count; i++) {
            CCollision collision;
            CPathNode* node = cell.nodes[i];
            const CAIFilterRealPosition& position = node->GetPosition();
            const CVector3& center = CVector3(position.x, position.y, position.z);
            sphere.SetCenter(center);
            if (sphere.TestCollision(*volume, collision, false) != 1) {
                node->m_enabled = 0;
                changed = true;
            }
        }
        return changed;
    }

    void DisableCollidingNodes(const IVolume* volume);

    static dwi::hash_map<unsigned int, PathNodes> s_pathNodeMap;
};

void CPathNodeList::DisableCollidingNodes(const IVolume* volume) {
    CVector3 min;
    CVector3 max;
    volume->GetExtents(min, max);
    PathNodes cell;
    unsigned int key = MakeKey(min);
    dwi::hash_map<unsigned int, PathNodes>::iterator it = s_pathNodeMap.find(key);
    if (it != s_pathNodeMap.end()) {
        cell = it->second;
        if (DisableNodes(cell, volume)) {
            cell.marked = false;
            s_pathNodeMap.erase(it);
            s_pathNodeMap.insert(key, cell);
        }
    }
    key = MakeKey(max);
    dwi::hash_map<unsigned int, PathNodes>::iterator it2 = s_pathNodeMap.find(key);
    if (it2 != s_pathNodeMap.end() && it2->second != cell) {
        cell = it2->second;
        if (DisableNodes(cell, volume)) {
            cell.marked = false;
            s_pathNodeMap.erase(it2);
            s_pathNodeMap.insert(key, cell);
        }
    }
}
