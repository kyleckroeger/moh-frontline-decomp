// A fragment of sound.cpp (0x800ea4e4): the music wrappers over the path
// player. MUSIC_Fade fades to a level scaled by the music volume (percent;
// 100.0f is an entry of the file's .sdata2 pool), MUSIC_SendEvent sends the
// project's event (project id in the high byte) to the music handle,
// MUSIC_SetLatency and MUSIC_SetLevel clamp the latency (50 to 5000) and
// control level (0 to 127) before passing them on, MUSIC_Pause pauses or
// resumes every path when a music handle exists, MUSIC_Stop stops the
// music handle, and MUSIC_Start (with a music handle) sets the clamped
// level and latency, the volume from the squared music volume (percent, as a
// signed byte), and jumps every path to the given position, reporting a
// failure through err (its message is a .rodata pool entry). The file name is this project's; the original record is
// sound.cpp (sound.cpp holds the sound-stream wrappers). The functions and
// globals are named by their symbols; the result types and the path player
// prototypes as seen from this file are inferred.
extern "C" {
int PATH_fade(unsigned int, int, int);
int PATH_event(unsigned int, short, int);
int PATH_latency(unsigned int, int);
int PATH_control(unsigned int, int);
void PATH_pause(unsigned int, signed char);
void PATH_stop(unsigned int);
int PATH_volume(unsigned int, int);
int PATH_jump(unsigned int, int, int);
}

void err(char*, ...);

extern int gMusicHandle;
extern int gMusicProjectID;
// The file's private music volume.
extern float g_musicVolume;

void MUSIC_Fade(float level, int time) {
    PATH_fade(gMusicHandle, 100.0f * (level * g_musicVolume), time);
}

bool MUSIC_SendEvent(int event) {
    PATH_event(gMusicHandle, (gMusicProjectID << 8) | event, 1);
    return true;
}

bool MUSIC_SetLatency(int latency) {
    PATH_latency(gMusicHandle, latency < 50 ? 50 : (latency > 5000 ? 5000 : latency));
    return true;
}

bool MUSIC_SetLevel(int level) {
    PATH_control(gMusicHandle, level < 0 ? 0 : (level > 127 ? 127 : level));
    return true;
}

bool MUSIC_Pause(bool pause) {
    if (gMusicHandle < 0)
        return false;
    PATH_pause(0xFFFFFFFF, pause != 0);
    return pause;
}

void MUSIC_Stop() {
    if (gMusicHandle >= 0)
        PATH_stop(gMusicHandle);
}

void MUSIC_Start(int level, int latency, int position) {
    if (gMusicHandle >= 0) {
        PATH_control(gMusicHandle, level < 0 ? 0 : (level > 127 ? 127 : level));
        PATH_latency(gMusicHandle, latency < 50 ? 50 : (latency > 5000 ? 5000 : latency));
        PATH_volume(gMusicHandle, (signed char)(100.0f * (g_musicVolume * g_musicVolume)));
        int result = PATH_jump(0xFFFFFFFF, position, 1);
        if (result < 0)
            err("PATH_jump Failed (%d)", result);
    }
}
