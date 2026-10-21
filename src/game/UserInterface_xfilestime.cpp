// A fragment of UserInterface.cpp (0x800bade0): UpdateXFilesText (until the
// X-files text is done: the landmark and time stamps are drawn while shown,
// and once the time stamp is parsed the text alpha fades by 1/60-ish of the
// fade rate, a function static of 21, times the frame time; below zero the
// fade and the text end, at exactly zero they restart at 63),
// SetXFilesTextOnscreenTime (the on-screen time of the X-files text, file-local
// in the original, declared extern here), SetTimeStampString and
// SetLandmarkStampString (copy the mission's time and landmark stamp texts
// into 48-byte buffers; a null text or a single blank marks the stamp as done
// and empties its buffer, the time stamp also ending the fade and the X-files
// text, and a landmark text also turns its display on). The classes,
// functions and globals are named by the symbols; the blank is held in a
// local pointer (the original loads it before the text argument). The rest of
// the file is in other units.
extern "C" {
char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
}

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

void DisplayLandmarkStamp(float);
void DisplayTimeStamp(float);

void UpdateXFilesText(float dt) {
    if (!bDoneShowingXFilesText) {
        if (bDisplayLandmarkStamp)
            DisplayLandmarkStamp(dt);
        if (bDisplayTimeStamp)
            DisplayTimeStamp(dt);
        if (bDoneParsingTimeStamp && !bDoneFading) {
            static float xxxFadeValue = 21.0f;
            alpha -= 0.016683351f * (xxxFadeValue * dt);
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
