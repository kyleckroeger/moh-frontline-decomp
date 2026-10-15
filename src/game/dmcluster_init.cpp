// A fragment of dmcluster.cpp (0x800f5290): DMClusterBindSkeleton keeps a
// cluster object's skeleton (+12) and DMClusterInitObject its count and two
// matrix arrays (+0, +4, +8). DMClusterObject_T and CharSkel_t are named by
// the mangled symbols; the members are inferred from offsets. The rest of the
// file is in other units.
struct CharSkel_t;

struct DMClusterObject_T {
    unsigned long count;
    float (*matrices)[3][4];
    float (*matrices2)[3][4];
    CharSkel_t* skeleton;
};

void DMClusterBindSkeleton(DMClusterObject_T* object, CharSkel_t* skeleton) {
    object->skeleton = skeleton;
}

void DMClusterInitObject(DMClusterObject_T* object, float (*matrices)[3][4], float (*matrices2)[3][4],
                         unsigned long count) {
    object->count = count;
    object->matrices = matrices;
    object->matrices2 = matrices2;
}
