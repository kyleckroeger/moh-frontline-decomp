// mainloop.cpp: its initialised data (.data): g_idSet, an acquire set holding the indices 0 to
// 3 (the file record's first .data object, right after camera.cpp's). The class is a view
// holding only what the definition needs. Checked by research/tools/check_data.py.
// (camera.cpp's CCamera::ASPECT_4x3, formerly here, is defined by the merged camera unit.)

class CWorldObject {
public:
    struct SAcquireSet {
        int m_indices[4];
    };
};

CWorldObject::SAcquireSet g_idSet = {{0, 1, 2, 3}};
