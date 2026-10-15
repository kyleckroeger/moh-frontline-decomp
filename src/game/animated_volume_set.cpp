// A fragment of animated_volume.cpp (0x800c0208): CAnimatedVolume::Set keeps the location-collision data (+36) and map
// (+40) and clears the flag at +32 (the members are inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct CObjLocationCollisionData;
struct CObjLocationCollisionMap;

class CAnimatedVolume {
public:
    void Set(CObjLocationCollisionData*, CObjLocationCollisionMap*);

    unsigned char unknown00[32];
    bool m_flag20;
    unsigned char unknown21[3];
    CObjLocationCollisionData* m_data;
    CObjLocationCollisionMap* m_map;
};

void CAnimatedVolume::Set(CObjLocationCollisionData* data, CObjLocationCollisionMap* map) {
    m_data = data;
    m_map = map;
    m_flag20 = false;
}
