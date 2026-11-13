// A fragment of UserInterface.cpp (0x800bafdc), which is compiled with
// deferred inlining, so its functions are in reverse source order:
// InitTimeStampStuff (empties the landmark and time stamp texts and their
// display copies, loads the level's font file and creates the grey timestamp
// font at unit scale, at the screen's maximum depth when there is a screen)
// and ResetTimeStampStuff (deletes that font, clears the X-Files text time,
// empties the four texts and resets the typing markers and the stamp and fade
// flags). The four texts belong to the file's .bss block (after teamColors and
// TempVerts) and are reached from its base register, so the block is defined
// here with size views and pooled; the UserInterface.cpp unit places it. The
// flags, markers, time and font are .sbss globals of the file placed by
// UserInterface_xfilestime.cpp. CFont, CScreen, CColor and the TLT loaders
// are named by the symbols; the font's layout (table at +28) and the level
// resource view are inferred.
extern "C" {
void* memset(void*, int, unsigned long);
}

#include "game/CScreen.h"


enum ERenderPriority {};

/* inferred: the level resource's file names (the font file at +20) */
struct LevelFileNamesView {
    unsigned char unknown00[20];
    const char* fontFile;
};
struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelFileNamesView* m_data;
};
enum TLTResourceID {};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
const char* TLT_LoadFileFromLevelBigFile(const char*, int*);

class CRenderBin {
public:
    unsigned char unknown00[28];
    virtual ~CRenderBin();
};

class CFont : public CRenderBin {
public:
    virtual ~CFont();
    CFont(const char*, ERenderPriority);
    void SetColor(CColor);
    void SetScale(float, float);
    void SetDepth(unsigned long);
    unsigned char unknown20[8];
};

/* view: size from the original */
struct _spritepolyvert {
    unsigned char data[0x18];
};

/* view: teamColors is a CColor[4] the UserInterface.cpp unit constructs;
   a constructor-less view keeps this fragment from compiling an initialiser */
struct TeamColorsView {
    unsigned char rgba[4][4];
};

TeamColorsView teamColors;
_spritepolyvert TempVerts[24];
char landmark[48];
char timestamp[48];
char landmarkDisplayText[48];
char timestampDisplayText[48];

extern float onscreenTime;
extern CFont* timestampFont;
extern int charMarkerForLandmarkString;
extern int charMarkerForTimeStampString;
extern bool bDisplayLandmarkStamp;
extern bool bDisplayTimeStamp;
extern bool bDoneParsingLandmark;
extern bool bDoneParsingTimeStamp;
extern bool bDoneFading;
extern bool bDoneShowingXFilesText;

void InitTimeStampStuff() {
    charMarkerForLandmarkString = 0;
    memset(landmark, 0, 48);
    memset(timestamp, 0, 48);
    memset(landmarkDisplayText, 0, 48);
    memset(timestampDisplayText, 0, 48);
    LevelFileContentsStruct_* level = TLT_FindNextResourceByType((TLTResourceID)11, 0);
    const char* file = TLT_LoadFileFromLevelBigFile(level->m_data->fontFile, 0);
    timestampFont = new CFont(file, (ERenderPriority)111);
    timestampFont->SetColor(CColor(128, 128, 128, 128));
    timestampFont->SetScale(1.0f, 1.0f);
    CScreen* screen = CScreen::GetCurrent();
    if (screen)
        timestampFont->SetDepth(screen->GetDepthBufferMax());
}

void ResetTimeStampStuff() {
    if (timestampFont)
        delete timestampFont;
    onscreenTime = 0.0f;
    memset(landmark, 0, 48);
    memset(timestamp, 0, 48);
    memset(landmarkDisplayText, 0, 48);
    memset(timestampDisplayText, 0, 48);
    charMarkerForLandmarkString = 0;
    charMarkerForTimeStampString = 0;
    bDisplayLandmarkStamp = false;
    bDisplayTimeStamp = false;
    bDoneParsingLandmark = false;
    bDoneParsingTimeStamp = false;
    bDoneFading = false;
    bDoneShowingXFilesText = false;
}
