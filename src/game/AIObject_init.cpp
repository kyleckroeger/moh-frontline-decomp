// A fragment of AIObject.cpp (0x8004ebe0): CAIObject::Init links the object at the head of the global AI object list
// and counts it; Update and PreUpdate are empty. The list head and count are
// file-local in the original and declared extern here; the link at +4 is
// inferred. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CAIObject {
public:
    void Init();
    void Update(float);
    void PreUpdate();

    unsigned char unknown00[4];
    CAIObject* m_next;
};

extern CAIObject* g_paioAIObjectListStart; // file-local in the original
extern int g_iAIObjectCount;               // file-local in the original

void CAIObject::Init() {
    m_next = g_paioAIObjectListStart;
    g_paioAIObjectListStart = this;
    g_iAIObjectCount++;
}

void CAIObject::Update(float) {
}

void CAIObject::PreUpdate() {
}
