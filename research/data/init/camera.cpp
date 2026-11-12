// camera.cpp: its initialised data (.data, .sdata2): CCamera's 4:3 aspect ratio and
// g_idSet, an acquire set holding the indices 0 to 3. The classes are views holding only
// what these definitions need. Checked by research/tools/check_data.py.

class CCamera {
public:
    static const float ASPECT_4x3;
};

class CWorldObject {
public:
    struct SAcquireSet {
        int m_indices[4];
    };
};

const float CCamera::ASPECT_4x3 = 0.75f;

CWorldObject::SAcquireSet g_idSet = {{0, 1, 2, 3}};
