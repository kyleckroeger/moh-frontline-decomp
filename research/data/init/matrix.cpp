// matrix.cpp: its initialised data (.data): _Mat_Data, the identity matrix. A file static
// in the original, defined global here because the file's fragments reach it with extern
// (as const CMatrix). CMatrix is a view: 4 rows of 4 floats.
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/matrix.json

struct CMatrix { // view: size and identity layout from the original
    float m[4][4];
};

CMatrix _Mat_Data = {{ // file-local in the original
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
}};
