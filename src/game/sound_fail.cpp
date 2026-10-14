// A fragment of sound.cpp (0x800ec3e0); the file name is this project's.
// AEMS_Pause starts the pause AEMS event (event 13) when pausing and ends it
// when resuming, inside the sound system's critical section, and returns
// whether the pause event is playing. Then the file's failure reporting hook,
// fail: a variadic function whose body is empty in this build. AEMS_Pause, its
// static handle, the AEMS hooks and fail (local to the file) are named by the
// symbols; the event block size, AEMS_Pause's return type and fail's parameter
// names are not known. The functions around these are not part of this unit.
extern "C" {
extern int (*SNDAEMS_beginevent)(int*);
extern int (*SNDAEMS_endevent)(int, int*);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
}

bool AEMS_Pause(bool pause) {
    static int handle = -1;
    int event[4];
    event[0] = 13;
    SNDSYS_entercritical();
    if (pause) {
        handle = SNDAEMS_beginevent(event);
    } else if (handle != -1) {
        handle = SNDAEMS_endevent(handle, event);
    }
    SNDSYS_leavecritical();
    return !(handle < 0);
}

static void fail(char*, ...) {
}
