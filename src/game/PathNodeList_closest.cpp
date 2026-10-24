// A fragment of PathNodeList.cpp (0x800550d8): CPathNodeList's
// GreedySearchAwayFromPoint starts at the node closest to the position and
// keeps moving to the neighbour farthest from the point to avoid while that
// is farther and the distance is still below the wanted one, returning the
// node reached when it is far enough (else none); the node's neighbour count
// (+16) and neighbour indices (+20) are inferred, as is the pointer through
// which it copies and measures each position. CPathNodeList::GetClosestNode
// (position, ignore collision) looks up the path-node map entry for the
// position's 5-unit grid cell (the cell's key packs the y and x cell indices,
// offset by 16384, as 15-bit fields) and returns the closest enabled node of
// that cell within 5.0, skipping nodes whose line to the position collides
// unless collisions are ignored (none when the cell has no entry). The
// classes, the template and the functions are named by the mangled symbols;
// the node view (enabled flag bit 7 of +17), the map's find and iterator
// (as in publisher_reset.cpp), the inline cell-key helper and distance, and
// the copy of each node's position it works on are inferred. The rest of the
// file is in other units. The second GetClosestNode(position, direction)
// prefers, among the cell's enabled nodes ahead (direction dot product above
// 0.01), the one with the least squared distance over that product, else the
// nearest of the rest (squared distance, both below 25 and with a clear line);
// GetPathNodeByID indexes the node array (+0).
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
        float dy;
        float dz;
        float dx;
        dx = x - other.x;
        dy = y - other.y;
        dz = z - other.z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    float x;
    float y;
    float z;
};

class CAIFilterObject;

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    float w;
} __attribute__((aligned(8)));

class CAIFilterGlobal {
public:
    static bool CheckCollisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                                CAIFilterObject*);
};

/* inferred: a path node (24 bytes) */
class CPathNode {
public:
    CAIFilterRealPosition m_position;
    unsigned char unknown0c[4];
    unsigned char m_neighbourCount;
    unsigned char m_enabled : 1;
    unsigned char m_flags : 7;
    unsigned char unknown12[2];
    unsigned short* m_neighbours;
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
    CPathNode* GetClosestNode(const CAIFilterRealPosition&, const CAIFilterRealVector3&) const;
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

    CPathNode* GetPathNodeByID(int) const;
    CPathNode* GreedySearchAwayFromPoint(const CAIFilterRealPosition&, const CAIFilterRealPosition&, float) const;

    static dwi::hash_map<unsigned int, PathNodes> s_pathNodeMap;

    CPathNode* m_nodes;
};

CPathNode* CPathNodeList::GreedySearchAwayFromPoint(const CAIFilterRealPosition& from,
                                                    const CAIFilterRealPosition& away, float distance) const {
    CPathNode* node = GetClosestNode(from, false);
    if (!node)
        return 0;
    CAIFilterRealPosition position = node->m_position;
    CAIFilterRealPosition* p = &position;
    float best = p->GetDistance(away);
    bool moved = true;
    while (best < distance && moved) {
        moved = false;
        unsigned short* neighbour = node->m_neighbours;
        unsigned short* end = neighbour + node->m_neighbourCount;
        for (; neighbour < end; neighbour++) {
            CPathNode* other = &m_nodes[*neighbour];
            *p = other->m_position;
            float d = p->GetDistance(away);
            if (d > best) {
                best = d;
                node = other;
                moved = true;
            }
        }
    }
    if (best >= distance)
        return node;
    return 0;
}

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

CPathNode* CPathNodeList::GetClosestNode(const CAIFilterRealPosition& position,
                                         const CAIFilterRealVector3& direction) const {
    dwi::hash_map<unsigned int, PathNodes>::iterator it = s_pathNodeMap.find(MakeKey(position));
    if (it == s_pathNodeMap.end())
        return 0;
    float bestAhead = 25.0f;
    int count = it->second.count;
    CPathNode** nodes = it->second.nodes;
    CPathNode* ahead = 0;
    CPathNode* closest = 0;
    float best = bestAhead;
    for (int i = 0; i < count; nodes++, i++) {
        CPathNode* node = *nodes;
        if (node->m_enabled) {
            CAIFilterRealPosition copy __attribute__((aligned(8))) = node->m_position;
            float dx = copy.x - position.x;
            float dy = copy.y - position.y;
            float dz = copy.z - position.z;
            float dot = dx * direction.x + dy * direction.y + dz * direction.z;
            if (dot > 0.01) {
                float score = (dx * dx + dy * dy + dz * dz) / dot;
                if (score < bestAhead && !CAIFilterGlobal::CheckCollisionLineToEnvironment(copy, position, 0)) {
                    bestAhead = score;
                    ahead = node;
                }
            } else {
                float d = dx * dx + dy * dy + dz * dz;
                if (d < best && !CAIFilterGlobal::CheckCollisionLineToEnvironment(copy, position, 0)) {
                    best = d;
                    closest = node;
                }
            }
        }
    }
    if (ahead)
        return ahead;
    return closest;
}

CPathNode* CPathNodeList::GetPathNodeByID(int id) const {
    return &m_nodes[id];
}
