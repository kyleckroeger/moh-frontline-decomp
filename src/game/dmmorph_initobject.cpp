// A fragment of dmmorph.cpp (0x800f7620): DMMorphInitObject stores a morph
// object's target count (+256) and clears that many weights. DMMorphObject_T
// is named by the mangled symbol; its layout (64 float weights, then the
// count) is inferred. The rest of the file is in other units.
struct DMMorphObject_T {
    float weights[64];
    unsigned long count;
};

void DMMorphInitObject(DMMorphObject_T* object, unsigned long count) {
    object->count = count;
    for (unsigned long i = 0; i < object->count; i++)
        object->weights[i] = 0.0f;
}
