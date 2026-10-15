// A fragment of animplayer.cpp (0x8010a39c): CAnimatedPlayer::Draw draws the
// player as a soldier unless the draw context's value at +0x114 is the one
// held by the object at +0x44b0 (at its +0x768). The classes and function are
// named by the mangled symbols; the members are inferred from the offsets,
// and CAnimatedPlayer is a non-virtual view deriving from CSoldierObject. The
// rest of the file is in other units.
class CDrawContext {
public:
    unsigned char unknown000[0x114];
    int m_value114;
};

/* Inferred: the object at +0x44b0 and its value at +0x768. */
struct AnimatedPlayerLinkView {
    unsigned char unknown000[0x768];
    int m_value768;
};

class CSoldierObject {
public:
    void Draw(CDrawContext&);
};

class CAnimatedPlayer : public CSoldierObject {
public:
    void Draw(CDrawContext&);

    unsigned char unknown0000[0x44b0];
    AnimatedPlayerLinkView* m_link;
};

void CAnimatedPlayer::Draw(CDrawContext& context) {
    if (context.m_value114 != m_link->m_value768)
        CSoldierObject::Draw(context);
}
