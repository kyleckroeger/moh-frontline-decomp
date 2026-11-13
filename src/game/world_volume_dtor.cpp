// A fragment of world_volume.cpp (0x800cc124): the CWorldVolume destructor (its
// virtual table pointer; the sub-volume list at +4 destroyed: its nodes
// returned to the free list, the sentinel and count reset and owned node
// storage freed; then the inline IVolume destructor; the object freed when
// asked). The file name is this project's; the original record is
// world_volume.cpp, before world_volume_list.cpp. IVolume, CWorldVolume,
// CCDBObject and the dwi::list template are named by the mangled symbols; the
// list view is world_volume_list.cpp's (its destructor inline here, with
// the loop locals declared node first, as in world_object_dtor.cpp), and only
// the virtuals the destructor needs are declared. CWorldVolume declares its
// TestCollision override for IVolume (defined elsewhere) first so its global
// virtual table is not emitted here. IVolume's destructor is inline and its
// table weak in the original, so the compiler's copies are weak duplicates,
// linked to the original copies.
class CCDBObject;
class CCollision;

extern "C" void MEM_free(void*);

#include "game/DwiContainers.h"

class IVolume {
public:
    virtual ~IVolume() {}
};

class CWorldVolume : public IVolume {
public:
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual ~CWorldVolume();

    dwi::list<const CCDBObject*> m_subVolumes;
};

CWorldVolume::~CWorldVolume() {
}
