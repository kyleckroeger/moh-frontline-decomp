/* A fragment of the music path player's path.c (0x8010fb0c):
   PATHI_setfadevolume moves a fading track's volume along its fade: the fade
   comes from the state's table (the track's curve, from 1) or is the state's
   default shape and duration (reversed when fading up); the elapsed share of
   the duration selects the volume between the start and target levels
   (shape 1 linear, 2 quadratic, 3 a 1/(25t) curve), scaled to 0..127, and a
   finished fade sets the base volume, stops a track faded to silence and
   clears the fade. PATHTRACK is named by the mangled symbol; its fields, the
   state view and the fade record are inferred from
   the code (as in path_stop.c); pfstate, milliseconds and the functions are
   named by their symbols. The rest of the file is in other units. */
struct PATHFADEVIEW {
    unsigned int shape : 7;
    unsigned int reverse : 1;
    unsigned int unknown1 : 8;
    unsigned int duration : 16;
};

struct PATHTRACK {
    unsigned char unknown00[4];
    unsigned int fadetime;
    signed char fadestart;
    signed char fadecurve;
    signed char fadetarget;
    unsigned char unknown0b[21];
    signed char basevolume;
    unsigned char unknown21;
    short node;
    unsigned char unknown24[24];
    int handle;
};

struct PATHSTATEVIEW {
    unsigned char unknown00[20];
    int fadeduration;
    int fadeshape;
    unsigned char unknown1c[40];
    PATHFADEVIEW* fades;
};

extern PATHSTATEVIEW* pfstate;
extern unsigned int milliseconds;

extern "C" void PATHX_volume(int, int);
extern "C" void PATHX_stop(int);

void PATHI_setfadevolume(PATHTRACK* track) {
    signed char start;
    signed char target;
    target = track->fadetarget;
    start = track->fadestart;
    float delta = (float)target - (float)start;
    PATHFADEVIEW fade;
    float t;
    float k = -1.0f;
    float low;

    if (track->fadecurve > 0) {
        fade = pfstate->fades[track->fadecurve - 1];
    } else {
        fade.duration = pfstate->fadeduration;
        fade.shape = pfstate->fadeshape;
        fade.reverse = delta < 0.0f ? 0 : 1;
    }
    t = (float)(milliseconds - track->fadetime) / (float)fade.duration;
    if (t >= 1.0f) {
        PATHX_volume(track->handle, track->basevolume);
        if (!track->fadetarget) {
            PATHX_stop(track->handle);
            track->node = -1;
        }
        track->fadestart = -1;
        track->fadetarget = -1;
        track->fadecurve = -1;
        track->fadetime = 0;
        track->basevolume = track->fadetarget;
        return;
    }
    if (fade.reverse)
        t = 1.0f - t;
    switch (fade.shape) {
    case 1:
        k = 1.0f - t;
        break;
    case 2:
        k = 1.0f - t * t;
        break;
    case 3:
        k = 1.0f / t / 25.0f;
        break;
    }
    low = start;
    if (target < start)
        low = target;
    if (delta < 0.0f)
        delta = -delta;
    if (k >= 0.0f && k <= 1.0f) {
        float volume = delta * k + low;
        volume = 1.27f * volume;
        PATHX_volume(track->handle, (int)volume);
    }
}
