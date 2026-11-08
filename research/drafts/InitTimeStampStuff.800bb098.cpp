extern "C" {
char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
void* memset(void*, int, unsigned long);
}

enum ERenderPriority {};

class CColor {
public:
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

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

class CScreen {
public:
    static CScreen* GetCurrent();
    unsigned long GetDepthBufferMax();
};

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

extern float onscreenTime; // file-local in the original
extern CFont* timestampFont;
extern int charMarkerForLandmarkString;
extern int charMarkerForTimeStampString;
extern bool bDisplayLandmarkStamp;
extern bool bDisplayTimeStamp;
extern bool bDoneParsingLandmark;
extern bool bDoneParsingTimeStamp;
extern bool bDoneFading;
extern bool bDoneShowingXFilesText;
extern char landmark[48];
extern char timestamp[48];
extern char landmarkDisplayText[48];
extern char timestampDisplayText[48];

void SetXFilesTextOnscreenTime(float time) {
    onscreenTime = time;
}

void SetTimeStampString(char* text) {
    if (text) {
        strcpy(timestamp, text);
        const char* blank = " ";
        if (!strcmp(text, blank)) {
            timestamp[0] = 0;
            bDoneParsingTimeStamp = true;
            bDoneFading = true;
            bDoneShowingXFilesText = true;
        }
    } else {
        timestamp[0] = 0;
        bDoneParsingTimeStamp = true;
        bDoneFading = true;
        bDoneShowingXFilesText = true;
    }
}

void SetLandmarkStampString(char* text) {
    if (text) {
        strcpy(landmark, text);
        bDisplayLandmarkStamp = true;
        const char* blank = " ";
        if (!strcmp(text, blank)) {
            landmark[0] = 0;
            bDoneParsingLandmark = true;
        }
    } else {
        landmark[0] = 0;
        bDoneParsingLandmark = true;
    }
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
    if (CScreen::GetCurrent())
        timestampFont->SetDepth(CScreen::GetCurrent()->GetDepthBufferMax());
}
