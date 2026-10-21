// A fragment of UserInterface.cpp (0x800ba8e4): the mission's time and
// landmark stamps. DisplayTimeStamp and DisplayLandmarkStamp draw the stamp
// text typed so far with the timestamp font (grey, text alpha) at a fixed
// position (function statics: x 40, y 316 or 296 times the screen scale) and
// reveal one more character every 0.028 s, playing a random typing sound
// (an AEMS event at the shell's effects volume) on every other character; an
// empty stamp counts as done (the landmark then starts the time stamp).
// UpdateXFilesText runs them until the X-files text is done and then fades
// the text alpha (below zero the text ends, at exactly zero it restarts at
// 63). SetXFilesTextOnscreenTime keeps the text's on-screen time (file-local
// in the original, declared extern here); SetTimeStampString and
// SetLandmarkStampString copy the stamp texts into 48-byte buffers (a null
// text or a single blank marks the stamp done and empties it; a landmark text
// turns its display on). The file was compiled with deferred inlining
// (functions emitted in reverse source order; the function statics are laid
// out in source order). The fade rate is a function static of an inferred
// static helper defined last: it is inlined into UpdateXFilesText and its own
// copy is discarded, which puts its static after the two stamp functions'
// statics as in the original. The classes, functions and globals are named
// by the symbols; the font, screen and shell views are inferred, the event
// templates are .rodata pool entries and the constants .sdata2 pool entries.
// The rest of the file is in other units.
extern "C" {
char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
unsigned long strlen(const char*);
void* memcpy(void*, const void*, unsigned long);
}

class CColor {
public:
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CFont {
public:
    void SetColor(CColor);
    void DrawText(float, float, const char*);
};

/* inferred: the screen's scale at +44 */
class CScreen {
public:
    unsigned char unknown000[44];
    float m_scale;
    unsigned char unknown030[224];
};

/* inferred: the shell's effects volume setting (+40, 1..7) */
class CShellMenu {
public:
    unsigned char unknown0000[40];
    unsigned char m_sfxSetting;
    unsigned char unknown0029[6611];
};

extern CScreen g_screen;
extern CShellMenu g_Shell;
float MathFunRandomReal(float, float);
void SoundGetVolumes(float&, float&);
int AEMS_SendEvent(void*);

extern float onscreenTime; // file-local in the original
extern bool bDisplayLandmarkStamp;
extern bool bDisplayTimeStamp;
extern bool bDoneParsingLandmark;
extern bool bDoneParsingTimeStamp;
extern bool bDoneFading;
extern bool bDoneShowingXFilesText;
extern char landmark[48];
extern char timestamp[48];
extern float alpha;

extern CFont* timestampFont;
extern int charMarkerForLandmarkString;
extern int charMarkerForTimeStampString;
extern char landmarkDisplayText[48];
extern char timestampDisplayText[48];

void DisplayLandmarkStamp(float);
void DisplayTimeStamp(float);

static float FadeValue();

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

void SetXFilesTextOnscreenTime(float time) {
    onscreenTime = time;
}

void UpdateXFilesText(float dt) {
    if (!bDoneShowingXFilesText) {
        if (bDisplayLandmarkStamp)
            DisplayLandmarkStamp(dt);
        if (bDisplayTimeStamp)
            DisplayTimeStamp(dt);
        if (bDoneParsingTimeStamp && !bDoneFading) {
            float fade = FadeValue();
            alpha -= 0.016683351f * (fade * dt);
            if (alpha < 0.0f) {
                bDoneFading = true;
                bDoneShowingXFilesText = true;
            }
            if (alpha == 0.0f) {
                bDoneFading = false;
                bDoneShowingXFilesText = false;
                alpha = 63.0f;
            }
        }
    }
}

void DisplayLandmarkStamp(float dt) {
    static float frame = 0.0f;
    static float xxxLandmarkX = 40.0f;
    static float xxxLandmarkY = 296.0f * g_screen.m_scale;
    if (!landmark[0]) {
        bDisplayTimeStamp = true;
        bDoneParsingLandmark = true;
    } else {
        timestampFont->SetColor(CColor(128, 128, 128, (int)alpha));
        timestampFont->DrawText(xxxLandmarkX, xxxLandmarkY, landmarkDisplayText);
        if (!bDoneParsingLandmark) {
            if (frame > 0.028) {
                int length = strlen(landmark);
                if (charMarkerForLandmarkString > length) {
                    charMarkerForLandmarkString = length;
                    bDisplayTimeStamp = true;
                    bDoneParsingLandmark = true;
                }
                memcpy(landmarkDisplayText, landmark, charMarkerForLandmarkString);
                frame = 0.0f;
                charMarkerForLandmarkString++;
                if (charMarkerForTimeStampString % 2 == 0) {
                    int sound = MathFunRandomReal(0.0f, 2.9999001f);
                    float sfx;
                    float music;
                    SoundGetVolumes(sfx, music);
                    sfx = (float)(g_Shell.m_sfxSetting - 1) / 6.0f;
                    sfx *= 0.25f;
                    int event[10] = {7, 3, 0, 0x1000, 0, 0, 0x9c4, 0, 0, 0};
                    event[2] = sound;
                    event[9] = 32767.0f * sfx;
                    AEMS_SendEvent(event);
                }
            }
            else {
                frame += 0.016683351f * dt;
            }
        }
    }
}

void DisplayTimeStamp(float dt) {
    static float frame = 0.0f;
    static float xxxTimeStampX = 40.0f;
    static float xxxTimeStampY = 316.0f * g_screen.m_scale;
    if (!timestamp[0]) {
        bDoneParsingTimeStamp = true;
    } else {
        timestampFont->SetColor(CColor(128, 128, 128, (int)alpha));
        timestampFont->DrawText(xxxTimeStampX, xxxTimeStampY, timestampDisplayText);
        if (!bDoneParsingTimeStamp) {
            if (frame > 0.028) {
                int length = strlen(timestamp);
                if (charMarkerForTimeStampString > length) {
                    charMarkerForTimeStampString = length;
                    bDoneParsingTimeStamp = true;
                }
                memcpy(timestampDisplayText, timestamp, charMarkerForTimeStampString);
                frame = 0.0f;
                charMarkerForTimeStampString++;
                if (charMarkerForTimeStampString % 2 == 0) {
                    int sound = MathFunRandomReal(0.0f, 2.9999001f);
                    float sfx;
                    float music;
                    SoundGetVolumes(sfx, music);
                    sfx = (float)(g_Shell.m_sfxSetting - 1) / 6.0f;
                    sfx *= 0.25f;
                    int event[10] = {7, 3, 0, 0x1000, 0, 0, 0x9c4, 0, 0, 0};
                    event[2] = sound;
                    event[9] = 32767.0f * sfx;
                    AEMS_SendEvent(event);
                }
            }
            else {
                frame += 0.016683351f * dt;
            }
        }
    }
}

/* inferred helper: the X-files text fade rate (a function static); inlined, its own copy is not linked */
static float FadeValue() {
    static float xxxFadeValue = 21.0f;
    return xxxFadeValue;
}
