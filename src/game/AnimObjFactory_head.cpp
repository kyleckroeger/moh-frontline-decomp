// The start of AnimObjFactory.cpp (0x80095ba0): CAnimObjectFactory::SetLargestObjectSize
// (rounded up to 16 bytes) and ReleaseObject (the object is pushed on the free
// list through the word at +44). The classes are views; members and their
// names are inferred from offsets. The rest of the file (AllocateObject
// onwards) is not part of this unit.
class CAnimObject {
public:
    unsigned char unknown00[44];
    CAnimObject* m_next;
};

class CAnimObjectFactory {
public:
    void SetLargestObjectSize(int size);
    void ReleaseObject(CAnimObject* object);

    void* m_defs;
    char* m_pool;
    CAnimObject* m_free;
    void* m_textures;
    int m_objectSize;
};

void CAnimObjectFactory::SetLargestObjectSize(int size) {
    if (size & 15)
        size += 16 - (size & 15);
    m_objectSize = size;
}

void CAnimObjectFactory::ReleaseObject(CAnimObject* object) {
    object->m_next = m_free;
    m_free = object;
}
