// A fragment of AnimObjFactory.cpp (0x800971bc): the CAnimObjectFactory
// constructor (the object size defaults to 17600) and the qsort comparison of
// object definitions by the word at +68. The classes are views; members and
// their names are inferred from offsets. Init, which comes before these
// functions, is not part of this unit.
class CAnimObject;

class CAnimObjDef {
public:
    CAnimObjDef();

    unsigned char unknown00[60];
    void* m_data;
    bool m_shared;
    unsigned char unknown41[3];
    int m_key;
    unsigned char unknown48[8];
};

class CAnimObjectFactory {
public:
    CAnimObjectFactory();

    CAnimObjDef* m_defs;
    char* m_pool;
    CAnimObject* m_free;
    void* m_textures;
    int m_objectSize;
    unsigned int m_numDefs;
    int m_maxObjects;
};

CAnimObjectFactory::CAnimObjectFactory() {
    m_objectSize = 17600;
}

int CompareObjectDefinitions(const void* a, const void* b) {
    int ka = ((const CAnimObjDef*)a)->m_key;
    int kb = ((const CAnimObjDef*)b)->m_key;
    if (ka < kb)
        return -1;
    return ka > kb;
}
