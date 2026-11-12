// Display-mesh render objects: lighting (the three camera-space light
// directions and colours and the doubled ambient colour, and scaling its
// alpha), binding a mesh, cluster and morph objects,
// matrices, scale and flags, and the part list rebuilt when the geometry
// state changes. DMRenderObject_T and the referenced types are named by the
// mangled symbols; the record's members are inferred from offsets and are not
// original.
extern "C" void* memset(void*, int, unsigned long);

#include "game/CVector3.h"


#include "game/CMatrix.h"


class CMatrixStack {
public:
    unsigned long m_size;
    int m_index;
    CMatrix* m_stack;

    CMatrix& Current() { return m_stack[m_index]; }
};

extern CMatrixStack* g_matStack;

/* Inferred: the light record is built by a four-float constructor (its
   loads are ordered z, y, x); a lighting setup (three light colours, the ambient colour, and
   three light directions). */
struct DMColorView {
    double pair[2];
};

struct DMLighting_T {
    DMColorView colors[3];
    DMColorView ambient;
    float directions[3][3];
};

struct DMLightView {
    DMLightView() {}
    DMLightView(float ax, float ay, float az, float aw) : x(ax), y(ay), z(az), w(aw) {}

    float x;
    float y;
    float z;
    float w;
};

class CDMesh;
struct DMMorphObject_T;
struct DMClusterObject_T;
struct TUInt128;

struct DMGeometryState_T {
    unsigned long dirty;
    unsigned char nodes[32];
};

struct DMGeomPartData_T {
    unsigned char data[256];
};

struct DMRenderObject_T {
    int field000;
    unsigned long flags;
    unsigned long pendingFlags;
    CDMesh* mesh;
    int field010;
    DMGeometryState_T geometry;
    unsigned char field038[8];
    CMatrix viewProj;
    CMatrix localToCamera;
    float (*clusterMatrices)[3][4];
    int field0C4;
    float scale[3];
    unsigned long reject2d;
    DMLightView lights[3];
    DMColorView lightColors[3];
    DMColorView ambient;
    unsigned char field148[8];
    void* parts;
    DMGeomPartData_T partData;
    DMClusterObject_T* cluster;
    DMMorphObject_T* morph;
    int field25C;
};

void* DMGeomBuildPartList(DMGeomPartData_T*, CDMesh*, DMGeometryState_T*);
void DMGeomResetState(DMGeometryState_T*);
void DMClusterShutdown();
void DMClusterInit();

// DMLightingScaleAlpha and DMLightingUpdate come first in the original file
// and are not part of this unit.

void DMLightingScaleAlpha(DMRenderObject_T* object, float scale) {
    ((float*)&object->ambient)[3] *= scale;
}

void DMLightingUpdate(DMRenderObject_T* object, DMLighting_T* lighting) {
    CVector3 direction0(lighting->directions[0][0], lighting->directions[0][1], lighting->directions[0][2]);
    CVector3 direction1(lighting->directions[1][0], lighting->directions[1][1], lighting->directions[1][2]);
    CVector3 direction2(lighting->directions[2][0], lighting->directions[2][1], lighting->directions[2][2]);
    CVector3 camera[3];
    g_matStack->Current().TransformVector(camera[0], direction0);
    g_matStack->Current().TransformVector(camera[1], direction1);
    g_matStack->Current().TransformVector(camera[2], direction2);
    for (int i = 0; i < 3; i++) {
        object->lights[i] = DMLightView(camera[i].x, camera[i].y, camera[i].z, 1.0f);
        object->lightColors[i] = lighting->colors[i];
    }
    object->ambient = lighting->ambient;
    float* ambient = (float*)&object->ambient;
    ambient[0] *= 2.0f;
    ambient[1] *= 2.0f;
    ambient[2] *= 2.0f;
}

void DMRenderSetClusterMatrices(DMRenderObject_T* object, float (*matrices)[3][4]) {
    object->clusterMatrices = matrices;
}

void DMRenderBindMorphObj(DMRenderObject_T* object, DMMorphObject_T* morph) {
    object->morph = morph;
}

void DMRenderBindClusterObj(DMRenderObject_T* object, DMClusterObject_T* cluster) {
    object->cluster = cluster;
}

void DMRenderUpdateParts(DMRenderObject_T* object) {
    if (object->mesh && object->geometry.dirty) {
        object->parts = DMGeomBuildPartList(&object->partData, object->mesh, &object->geometry);
        object->geometry.dirty = 0;
    }
}

void DMRenderSetScale(DMRenderObject_T* object, float x, float y, float z) {
    object->scale[0] = x;
    object->scale[1] = y;
    object->scale[2] = z;
}

void DMRenderSetLocalToCamera(DMRenderObject_T* object, CMatrix* localToCamera, CMatrix*) {
    object->localToCamera = *localToCamera;
}

void DMRenderSetViewProj(DMRenderObject_T* object, CMatrix* viewProj) {
    object->viewProj = *viewProj;
}

void DMRenderSet2dReject(DMRenderObject_T* object, unsigned long reject) {
    object->reject2d = reject != 0;
}

void DMRenderResetState(DMRenderObject_T* object) {
    memset(&object->viewProj, 0, 272);
    object->scale[0] = 1.0f;
    object->scale[1] = 1.0f;
    object->scale[2] = 1.0f;
}

void DMRenderBindModel(DMRenderObject_T* object, CDMesh* mesh) {
    if (object->mesh == mesh)
        return;
    object->mesh = mesh;
    object->geometry.dirty = -1;
    object->flags |= 0x411;
    object->pendingFlags |= 0x411;
}

void DMRenderInitObject(DMRenderObject_T* object, TUInt128*, TUInt128*, unsigned long) {
    memset(object, 0, sizeof(DMRenderObject_T));
    object->field000 = 1;
    object->mesh = 0;
    object->field010 = 0;
    object->cluster = 0;
    object->morph = 0;
    DMGeomResetState(&object->geometry);
    object->flags |= 0x410;
    object->pendingFlags |= 0x410;
}

void DMRenderSetFlags(DMRenderObject_T* object, unsigned long flags) {
    object->flags |= flags;
    object->pendingFlags |= flags;
}

void DMRenderShutdown() {
    DMClusterShutdown();
}

void DMRenderInit() {
    DMClusterInit();
}
