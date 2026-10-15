// A fragment of PathNodeList.cpp (0x800552e8): CPathNodeList::GetClosestNode
// (position, ignore collision) looks up the path-node map entry for the
// position's 5-unit grid cell (the cell's key packs the y and x cell indices,
// offset by 16384, as 15-bit fields) and returns the closest enabled node of
// that cell within 5.0, skipping nodes whose line to the position collides
// unless collisions are ignored (none when the cell has no entry). The
// classes, the template and the functions are named by the mangled symbols;
// the node view (enabled flag bit 7 of +17), the map's find and iterator
// (as in publisher_reset.cpp), the inline cell-key helper and distance, and
// the copy of each node's position it works on are inferred. The rest of the
// file is in other units.
extern "C" {
#include <math.h>
}

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition& operator=(const CAIFilterRealPosition& o) {
        float z_ = o.z;
        float y_ = o.y;
        float x_ = o.x;
        x = x_;
        y = y_;
        z = z_;
        return *this;
    }
    CAIFilterRealPosition(const CAIFilterRealPosition& o) {
        float z_ = o.z;
        float y_ = o.y;
        float x_ = o.x;
        x = x_;
        y = y_;
        z = z_;
    }

    float GetDistance(const CAIFilterRealPosition& other) const {
        float dz = z - other.z;
        float dy = y - other.y;
        float dx = x - other.x;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    float x;
    float y;
    float z;
};

class CAIFilterObject;

class CAIFilterGlobal {
public:
    static bool CheckCollisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                                CAIFilterObject*);
};

/* inferred: a path node (24 bytes) */
class CPathNode {
public:
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
        bool operator==(const iterator& other) const { return m_p == other.m_p; }
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

    value_type* m_data;
    int m_size;
    int m_count;
};
}

class CPathNodeList {
public:
    struct PathNodes {
        int count;
        CPathNode** nodes;
        bool marked;
    };

    CPathNode* GetClosestNode(const CAIFilterRealPosition&, bool) const;
    static unsigned int MakeKey(const CAIFilterRealPosition& p) {
        int cy = (int)((p.y + 2.5f) / 5.0f) + 16384;
        int cx = (int)((p.x + 2.5f) / 5.0f) + 16384;
        return ((cy & 0x7fff) << 16) + ((cx & 0x7fff) << 1);
    }
    static unsigned int MakeKey2(float x, float y) {
        int cy = (int)((y + 2.5f) / 5.0f) + 16384;
        int cx = (int)((x + 2.5f) / 5.0f) + 16384;
        return ((cy & 0x7fff) << 16) + ((cx & 0x7fff) << 1);
    }

    static dwi::hash_map<unsigned int, PathNodes> s_pathNodeMap;
};

CPathNode* CPathNodeList::GetClosestNode(const CAIFilterRealPosition& position, bool ignoreCollision) const {
    dwi::hash_map<unsigned int, PathNodes>::iterator it = s_pathNodeMap.find(MakeKey(position));
    if (it == s_pathNodeMap.end())
        return 0;
    int count = it->second.count;
    float best = 5.0f;
    CPathNode** nodes = it->second.nodes;
    CPathNode* closest = 0;
    for (int i = 0; i < count; nodes++, i++) {
        CPathNode* node = *nodes;
        if (node->m_enabled) {
            CAIFilterRealPosition copy __attribute__((aligned(8))) = node->m_position;
            float d = copy.GetDistance(position);
            if (d < best) {
                bool blocked;
                if (ignoreCollision)
                    blocked = false;
                else
                    blocked = CAIFilterGlobal::CheckCollisionLineToEnvironment(copy, position, 0);
                if (!blocked) {
                    best = d;
                    closest = node;
                }
            }
        }
    }
    return closest;
}
