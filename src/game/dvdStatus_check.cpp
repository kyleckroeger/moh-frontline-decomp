// A fragment of dvdStatus_base.c (0x80108e18; compiled as C++): CheckDVDError
// polls the drive until its state settles - honouring the reset button (wait
// for its release, clear the module area, hot reset) and a held Start+B+X on
// any pad (reset after half a second) - and shows a message for the fatal
// states (no disc, cover open, wrong disc, read error, fatal error) through
// ShowDVDError until the drive recovers; it returns 2 after an error was shown
// (the screen is blacked out again), else 0. The names come from the symbols
// and strings; parameter and local names are inferred. The pad check is an
// inferred inline helper that owns the two function statics (it gives the
// original's loop registers). The rest of the file is in other units.
extern "C" {
#include <stdio.h>
#include <string.h>
}
#include <dolphin/os.h>
#include <dolphin/pad.h>
#include <dolphin/vi.h>
#include <dolphin/dvd.h>

static int got_reset;
/* file-local in the original; declared extern here (in another unit) */
void ShowDVDError(int error);

static inline void CheckPadReset(PADStatus* pads) {
    static u8 got_reset[4] = { 0 };
    static u32 reset_time[4] = { 0 };
    int i;

    for (i = 0; i < 4; i++) {
        if (pads[i].err == PAD_ERR_NONE) {
            if ((pads[i].button & 0x1600) == 0x1600) {
                u32 now = OSTicksToMilliseconds(OSGetTime());
                if (!got_reset[i])
                    reset_time[i] = now;
                if (now - reset_time[i] > 500)
                    OSResetSystem(OS_RESET_HOTRESET, 1, FALSE);
                got_reset[i] = 1;
            } else {
                got_reset[i] = 0;
            }
        }
    }
}

extern "C" int CheckDVDError() {
    int shown = 0;
    s32 last = -9999;
    s32 status;
    PADStatus pads[4];

    for (;;) {
        do {
            status = DVDGetDriveStatus();
            if (OSGetResetButtonState())
                ::got_reset = 1;
            if (::got_reset) {
                while (OSGetResetButtonState())
                    ;
                printf("Reset button released. Reset NOW.\n");
                memset((void*)0x80003100, 0, 0x4000);
                OSResetSystem(OS_RESET_HOTRESET, 1, FALSE);
            }
            PADRead(pads);
            CheckPadReset(pads);
        } while (status == last);
        if (status != DVD_STATE_END && status != DVD_STATE_BUSY)
            printf("CheckDVDError: Trapped error %d\n", status);
        last = status;
        switch (status) {
        case DVD_STATE_FATAL_ERROR:
        case DVD_STATE_NO_DISK:
        case DVD_STATE_COVER_OPEN:
        case DVD_STATE_WRONG_DISK:
        case DVD_STATE_RETRY:
            ShowDVDError(status);
            shown = 2;
            continue;
        case DVD_STATE_WAITING:
        case DVD_STATE_COVER_CLOSED:
        case DVD_STATE_MOTOR_STOPPED:
        case DVD_STATE_PAUSING:
        case DVD_STATE_CANCELED:
        case DVD_STATE_IGNORED:
            printf("CheckDVDError: Ignoring %d\n", status);
            continue;
        case DVD_STATE_BUSY:
            if (!shown)
                break;
            printf("CheckDVDError: Trapped error %d. Previous error condition, \n", status);
            printf("\tso we're waiting for everything to chill out.\n");
            continue;
        }
        break;
    }
    if (shown) {
        VISetBlack(TRUE);
        VIFlush();
    }
    return shown;
}

