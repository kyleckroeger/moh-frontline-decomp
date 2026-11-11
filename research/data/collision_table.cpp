// g_theCollisionTable (.rodata 0x801707a0, 0x1b3 bytes): what the scene's sweep does
// with a pair of collision types. There are 29 types and the table is the lower
// triangle of a symmetric 29x29 matrix: the pair (a, b) with a <= b is entry
// a + b * (b + 1) / 2, so row b below lists a = 0..b. CScene::SweepExtents reads it
// (GetCollisionOrder in research/drafts/SweepExtents.800d9e48.cpp). 0 means the pair is
// never queued for a collision test; otherwise the value is stored as the pair's order,
// and the query passes order - 1 on to the collision (src/game/scene_query.cpp,
// scene_fast.cpp: m_order = order > 1 ? order - 1 : 0).
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/scene_visit.json

extern const signed char g_theCollisionTable[435];
const signed char g_theCollisionTable[435] = {
    /*  0 */ 0,
    /*  1 */ 1, 1,
    /*  2 */ 2, 3, 2,
    /*  3 */ 1, 0, 1, 0,
    /*  4 */ 1, 1, 0, 0, 0,
    /*  5 */ 2, 3, 2, 1, 1, 2,
    /*  6 */ 1, 0, 1, 0, 0, 0, 0,
    /*  7 */ 2, 3, 2, 0, 1, 2, 1, 2,
    /*  8 */ 0, 0, 0, 0, 1, 0, 1, 0, 0,
    /*  9 */ 1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
    /* 10 */ 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 11 */ 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 12 */ 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    /* 13 */ 2, 3, 2, 1, 0, 2, 1, 2, 0, 1, 0, 1, 0, 2,
    /* 14 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 15 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
    /* 16 */ 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
    /* 17 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 18 */ 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    /* 19 */ 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 20 */ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 21 */ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 22 */ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 23 */ 0, 1, 2, 1, 0, 2, 1, 2, 0, 1, 1, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 24 */ 1, 1, 3, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 25 */ 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 26 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 27 */ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 28 */ 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
