// A fragment of sound.cpp (0x800ea0e0); the file name is this project's. The
// file's message and error reporting hooks, msg and err: variadic functions
// whose bodies are empty in this build. Both are named by the symbols (local
// to the file); the parameter names are not known. Then the file-local sound
// user-data callback _userdata_client: for a type-1 callback with data, a
// leading 'SPIL' tag carries a lip-sync state for the scripted-sound
// speaker (g_objSound); otherwise the data is walked as tagged chunks (tag,
// size; all big-endian): 'NYSL' sets the speaker's lip-sync state, 'TBUS'
// shows its subtitle text (time 5.0) in one of _subtitle_rgb's colours
// (an optional one only when subtitles are enabled in g_Shell), and 'LUMS'
// (a SOUND_MULTISUBTITLEDATA header and its strings) shows the text for the
// shell's subtitle language, if present, and ends the walk; any other tag
// ends it too. The functions and globals are named by the symbols; the
// chunk layouts, the info record's fields and the shell flag are inferred,
// and the byte-order helpers are the inlined ones of sound_endian.cpp (the
// template one takes the chunk words through memory). _subtitle_rgb is the
// file's static colour table (sound.cpp). PlayScriptedSound before them is
// not part of this unit.
extern "C" unsigned long strlen(const char*);

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

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* inferred: the 'SMUL' chunk header (40 bytes) followed by its strings */
struct SOUND_MULTISUBTITLEDATA {
    unsigned int id;
    unsigned int size;
    unsigned int count;
    unsigned int optional;
    unsigned int color;
    unsigned int offsets[5];
};

void EndianSwap(SOUND_MULTISUBTITLEDATA&);
void DisplaySubtitle(const char*, float, CColor);

class CSoldierObject {
public:
    void SetFaceAnimationState(unsigned short, long);
};

extern CSoldierObject* g_objSound;

class CShellMenu {
public:
    unsigned int Get_LanguageSubtitleIndex();

    unsigned char unknown000[536];
    bool m_subtitles : 1;
};

extern CShellMenu g_Shell;

struct SNDUSERDATACBINFO {
    int type;
    char* data;
    char* base;
    long cookie;
};

extern CColor _subtitle_rgb[3];

static void msg(char*, ...) {
}

static void err(char*, ...) {
}

static void _userdata_client(SNDUSERDATACBINFO* info) {
    char* data = info->data;
    if (data) switch (info->type) {
    case 1: {
        char* p = data;
        unsigned long id = *(unsigned long*)p;
        ChangeEndian(id);
        if (id == 'SPIL') {
            unsigned short state = *(unsigned short*)(p + 4);
            ChangeEndian(state);
            g_objSound->SetFaceAnimationState(state, info->cookie);
            return;
        }
        while ((unsigned long)(p - info->base) < (unsigned long)data) {
            unsigned long size;
            id = ((unsigned long*)p)[0];
            size = ((unsigned long*)p)[1];
            ChangeEndian(id);
            ChangeEndian(size);
            switch (id) {
            case 'NYSL': {
                unsigned long state = ((unsigned long*)p)[2];
                ChangeEndian(state);
                g_objSound->SetFaceAnimationState(state, info->cookie);
                break;
            }
            case 'TBUS': {
                unsigned long optional = ((unsigned long*)p)[2];
                unsigned long color = ((unsigned long*)p)[3];
                ChangeEndian(optional);
                ChangeEndian(color);
                if (strlen(p + 16) && (!optional || g_Shell.m_subtitles))
                    DisplaySubtitle(p + 16, 5.0f, _subtitle_rgb[color]);
                break;
            }
            case 'LUMS': {
                SOUND_MULTISUBTITLEDATA subtitles = *(SOUND_MULTISUBTITLEDATA*)p;
                EndianSwap(subtitles);
                const char* strings = p + sizeof(SOUND_MULTISUBTITLEDATA);
                SOUND_MULTISUBTITLEDATA* header = &subtitles;
                unsigned int language = g_Shell.Get_LanguageSubtitleIndex();
                if (language < header->count) {
                    if (strlen(strings + header->offsets[language]) && (!header->optional || g_Shell.m_subtitles))
                        DisplaySubtitle(strings + header->offsets[language], 5.0f, _subtitle_rgb[header->color]);
                }
                return;
            }
            default:
                return;
            }
            p += size;
        }
    }
    }
}
