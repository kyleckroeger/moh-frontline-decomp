// A fragment of proxtrig.cpp (0x80045f84): CheckLeafsTriggers tests the point
// against each trigger listed by the BSP leaf, setting one bit per entry: an
// entry with its top bit set indexes a light volume, inside when the point is
// on the front of every plane; otherwise it indexes the trigger objects and,
// by the two flags, the trigger is tested (IsPointInTrigger) only when its core
// has the type and pointers the flags select (type 5 with the CRC 0x2070a66d
// in one case). The previous mask and leaf are kept in the proximity data.
// The function, structs and globals are named by the symbols; the members
// follow propdat_bsp.cpp where known and are otherwise inferred from offsets,
// as are the flag names and the light-volume test (an inferred inline
// helper). The rest of the file is in other units.
struct CVector3 {
    float x;
    float y;
    float z;
};

struct PropPlane4 {
    float x, y, z, d;
};

struct BPDLightVolume {
    float field00;
    float field04;
    float field08;
    float field0c;
    float field10;
    float field14;
    int planeCount;
    PropPlane4* planes;
    unsigned char unknown20[16];
};

struct MOH_core_Struct {
    unsigned char unknown00[12];
    int field0c;
    unsigned char unknown10[56];
    int field48;
    unsigned char unknown4c[12];
    unsigned long* field58;
    unsigned long* field5c;
    unsigned long* field60;
    unsigned long* field64;
    unsigned long* field68;
    void* field6c;
};

struct BSObject;

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    BSObject* bsObject;
};

struct _PropBSPLeaf {
    unsigned int triggerCount;
    unsigned char unknown04[28];
    unsigned short* triggers;
};

struct _ProximityData {
    unsigned int previousTriggers;
    unsigned int triggers;
    _PropBSPLeaf* previousLeaf;
    _PropBSPLeaf* leaf;
};

extern BPDLightVolume* g_pLightVolumes;
extern TriggerObject_struct g_pTriggerObjects[2000];

bool IsPointInTrigger(const CVector3&, const TriggerObject_struct*);

inline bool IsPointInLightVolume(const BPDLightVolume& volume, const CVector3& point) {
    PropPlane4* plane = volume.planes;
    for (int i = 0; i < volume.planeCount; i++, plane++) {
        if (plane->x * point.x + plane->y * point.y + plane->z * point.z + plane->d < 0.0)
            return false;
    }
    return true;
}

void CheckLeafsTriggers(const _PropBSPLeaf* leaf, const CVector3& point, _ProximityData* data, bool a, bool b) {
    unsigned int bit = 1;
    unsigned int inside = 0;
    for (unsigned int i = 0; i < leaf->triggerCount; i++) {
        unsigned short entry = leaf->triggers[i];
        unsigned int index = entry & 0x7fff;
        if ((entry >> 15) & 1) {
            if (IsPointInLightVolume(g_pLightVolumes[index], point))
                inside |= bit;
        } else if (a && !b) {
            TriggerObject_struct* trigger = &g_pTriggerObjects[index];
            MOH_core_Struct* core = trigger->core;
            if (core->field0c != 0 && (core->field60 || core->field64 || core->field6c) && IsPointInTrigger(point, trigger))
                inside |= bit;
        } else if (!b) {
            TriggerObject_struct* trigger = &g_pTriggerObjects[index];
            MOH_core_Struct* core = trigger->core;
            int type = core->field0c;
            if (type == 0 || core->field58 || core->field5c || core->field68) {
                if (IsPointInTrigger(point, trigger))
                    inside |= bit;
            } else if (type == 5) {
                if (core->field48 == 0x2070a66d && IsPointInTrigger(point, trigger))
                    inside |= bit;
            }
        }
        bit <<= 1;
    }
    data->previousTriggers = data->triggers;
    data->triggers = inside;
    data->previousLeaf = data->leaf;
    data->leaf = (_PropBSPLeaf*)leaf;
}
