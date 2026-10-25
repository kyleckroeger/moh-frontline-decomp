// dmgeom.cpp: DMGeomSetNodeState (a node's state byte changed and its bit set
// in the mask of changed nodes when the value differs), DMGeomResetState
// (every node reset to 0xff and marked changed, through a pointer to the node
// byte) and DMGeomBuildPartList (for each mesh node with a valid state, the
// parts of the chosen variant are listed, skipping empty part slots; the
// count of listed slots is returned).
// The functions and DMGeometryState_T, DMGeomPartData_T and CDMesh are named
// by the mangled symbols; the field names and the mesh's node, variant and
// part-index tables are descriptive views inferred from offsets.
struct DMGeometryState_T {
    unsigned long dirty;
    unsigned char nodes[32];
};

struct DMGeomPartData_T {
    unsigned short part;
    unsigned short flags;
    unsigned long data;
};

/* Inferred: a mesh node (its variants) and a variant (its parts). */
struct DMNodeView {
    unsigned char unknown00[16];
    unsigned short first;
    unsigned char count;
    unsigned char unknown13;
};

class CDMesh {
public:
    unsigned char unknown00[56];
    unsigned long m_nodeCount;
    DMNodeView* m_nodes;
    DMNodeView* m_variants;
    unsigned short* m_partIndices;
};

void DMGeomSetNodeState(DMGeometryState_T* state, long node, long value) {
    if (state->nodes[node] != value) {
        state->nodes[node] = value;
        state->dirty |= 1 << node;
    }
}

void DMGeomResetState(DMGeometryState_T* state) {
    for (int i = 0; i < 32; i++) {
        unsigned char* node = &state->nodes[i];
        *node = 0xff;
        state->dirty |= 1 << i;
    }
}

int DMGeomBuildPartList(DMGeomPartData_T* parts, CDMesh* mesh, DMGeometryState_T* state) {
    DMGeomPartData_T* part;
    int count;
    unsigned long i;
    unsigned short* index;
    DMNodeView* entry;
    count = 0;
    if (state) {
        for (i = 0; i < mesh->m_nodeCount; i++) {
            unsigned long variant = state->nodes[i];
            if (variant != 0xff && variant < mesh->m_nodes[i].count) {
                entry = &mesh->m_variants[variant + mesh->m_nodes[i].first];
                index = &mesh->m_partIndices[entry->first];
                part = parts;
                unsigned long total = entry->count;
                for (unsigned long n = 0; n < total; n++, parts++, count++) {
                    if (index[n] != 0xffff) {
                        part[n].part = index[n];
                        part[n].flags = 0;
                        part[n].data = 0;
                    }
                }
            }
        }
    }
    return count;
}
