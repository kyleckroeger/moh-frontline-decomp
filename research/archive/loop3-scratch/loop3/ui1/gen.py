import sys
holder_call=sys.argv[1]=='1'; outp=sys.argv[2]
src=open('../unlockB2/ui/init.cpp').read()
hdr=open('../unlockB2/ui/UserInterface_block.h').read()
src=src.replace('#include "UserInterface_block.h"\n','')
hdr=hdr.replace('static const char s_emptyPath[20] = "";\n','')
inline='''
static inline void LoadHeadshot(CSprite*& sprite, int i) {
    char path[20] = "";
    sprintf(path, "bitmaps\\\\Model%02d.gsh", i + 1);
    sprite = new CSprite(path, (CSprite::ESpriteLoadType)1, (ERenderPriority)0x6f);
}
'''
hdr=hdr.replace('1610612741u,3221225473u};\n}\n','1610612741u,3221225473u};\n}\n'+inline)
if holder_call:
    hdr=hdr.replace('void UserInterfaceBlockStrings() {\n','void UserInterfaceBlockStrings() {\n    LoadHeadshot(UserInterface::headshotImages[0], 0);\n')
old='''        char path[20] = "";
        for (int i = 0; i < 20; i++) {
            if (!headshotImages[i]) {
                sprintf(path, "bitmaps\\\\Model%02d.gsh", i + 1);
                headshotImages[i] = new CSprite(path, (CSprite::ESpriteLoadType)1, (ERenderPriority)0x6f);
            }
        }'''
assert old in src
src=src.replace(old,'''        for (int i = 0; i < 20; i++) {
            if (!headshotImages[i])
                LoadHeadshot(headshotImages[i], i);
        }''')
cut=src.index('static inline void LoadSprite')
open(outp,'w').write(src[:cut]+hdr+'\n'+src[cut:])
