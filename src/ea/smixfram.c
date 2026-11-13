// MIX_getframe: ask a mixer channel's source for its current frame (-1 when it
// has no frame callback). sndmix's layout is not known; the channel table
// (80-byte entries) is read through inferred offsets. MIX_getwetbuffer before
// it belongs to smixfx.c.
extern "C" char sndmix[];

extern "C" int MIX_getframe(int channel) {
    char* entry = *(char**)(sndmix + 500) + channel * 80;
    int (*getframe)(int) = *(int (**)(int))(entry + 52);

    if (getframe)
        return getframe(*(int*)(entry + 56));
    return -1;
}
