// UserInterface.cpp: its zero-initialised globals (0x346 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct _spritepolyvert { unsigned char data[0x18]; }; // view: size from the original
struct CSprite;
struct CFont;
class UserInterface {
public:
    static CSprite* healthSprite;
    static CSprite* compassSprite;
    static CSprite* ammoSprite;
    static CSprite* compassEdgeSprite;
    static CSprite* hitMeterSprite;
    static CSprite* blackSprite;
    static CSprite* dotSprite;
    static CSprite* greySprite;
    static CSprite* starSprite;
    static CSprite* circleSprite;
    static CFont* bulletText;
    static CFont* smallMPFont;
};

_spritepolyvert TempVerts[24];
char landmark[48];
char timestamp[48];
char landmarkDisplayText[48];
char timestampDisplayText[48];
CSprite* UserInterface::healthSprite;
CSprite* UserInterface::compassSprite;
CSprite* UserInterface::ammoSprite;
CSprite* UserInterface::compassEdgeSprite;
CSprite* UserInterface::hitMeterSprite;
CSprite* UserInterface::blackSprite;
CSprite* UserInterface::dotSprite;
CSprite* UserInterface::greySprite;
CSprite* UserInterface::starSprite;
CSprite* UserInterface::circleSprite;
CFont* UserInterface::bulletText;
CFont* UserInterface::smallMPFont;
float onscreenTime;
CFont* timestampFont;
int charMarkerForLandmarkString;
int charMarkerForTimeStampString;
bool bDisplayLandmarkStamp;
bool bDisplayTimeStamp;
bool bDoneParsingLandmark;
bool bDoneParsingTimeStamp;
bool bDoneFading;
bool bDoneShowingXFilesText;
