// A fragment of sprite.cpp (0x800827b0): CSpriteNoTextureBin's Link (empty) and Render (returns 0). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CDmaTag;
class CDmaPacket;

class CSpriteNoTextureBin {
public:
    void Link(CDmaTag*, CDmaPacket&);
    int Render(CDmaPacket&, void*);
};

void CSpriteNoTextureBin::Link(CDmaTag*, CDmaPacket&) {
}

int CSpriteNoTextureBin::Render(CDmaPacket&, void*) {
    return 0;
}
