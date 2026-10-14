// A fragment of animemot.cpp (0x8006438c): the weak EndianSwap(AnimFileFormat_t&),
// which byte-swaps the animation file header: two words, the object type, the
// file number, the frame count and a fourth short, then the state table, the
// animation table and the frame-offset table words. AnimFileFormat_t is named
// by the mangled symbol; its member views are inferred (as in animemot.cpp)
// and the byte-swapping inlines are those of propdat_core.cpp. The function is
// a weak copy of a header inline emitted in this file, so it is defined
// __declspec(weak). The rest of the file is not part of this unit.
struct AnimFileFormat_t {
    int field00;
    int field04;
    unsigned short objecttype;
    unsigned short filenum;
    unsigned short framecount;
    unsigned short field0e;
    int states;
    int animations;
    int frameOffsets;
};

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void ChangeEndian(unsigned int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<unsigned int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<unsigned int*>(bytes);
}

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

inline void EndianSwap(float& value, bool) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

inline void ChangeEndian(float& value) {
    float& f = value;
    EndianSwap(f, true);
}


__declspec(weak) void EndianSwap(AnimFileFormat_t& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field04);
    ChangeEndian(value.objecttype);
    ChangeEndian(value.filenum);
    ChangeEndian(value.framecount);
    ChangeEndian(value.field0e);
    ChangeEndian(value.states);
    ChangeEndian(value.animations);
    ChangeEndian(value.frameOffsets);
}
