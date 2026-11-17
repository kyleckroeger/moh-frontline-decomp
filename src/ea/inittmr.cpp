// REAL timer set-up: a periodic OS alarm posts a message to the timer thread,
// which runs the registered timer handlers.
#include <dolphin/os.h>
enum TIMERMSG {};

extern "C" {
void REAL_addexit(void (*)(void));
extern int TIMERhz;
extern volatile int ticks;
extern volatile int libticks;
extern void (*tmrsub[8])(void);
void TIMER_restore(void);
}

void ttInit();
void ttKill();
void ttMsg(TIMERMSG);

static OSAlarm Alarm;
static char bIsAlarmInited;
static char bIsTimerInited;

static void AlarmHandler(OSAlarm*, OSContext*);

void InitAlarm() {
    if (!bIsAlarmInited) {
        OSInitAlarm();
        bIsAlarmInited++;
    }
}

extern "C" int TIMER_init(int hz) {
    if (bIsTimerInited)
        return TIMERhz;

    if (hz == 0)
        hz = 100;
    TIMERhz = hz;

    OSTime now = OSGetTime();
    int period = (float)(*(unsigned int*)0x800000F8 / 4) * (1.0f / hz);
    ttInit();
    OSSetPeriodicAlarm(&Alarm, now, period, AlarmHandler);
    bIsTimerInited = 1;
    REAL_addexit(TIMER_restore);
    return TIMERhz;
}

extern "C" void TIMER_restore(void) {
    if (bIsTimerInited) {
        bIsTimerInited = 0;
        OSCancelAlarm(&Alarm);
        ttKill();
    }
}

void ttDoTimerMsg() {
    ticks++;
    libticks++;
    for (int i = 0; i < 8; i++) {
        if (tmrsub[i])
            tmrsub[i]();
    }
}

static void AlarmHandler(OSAlarm*, OSContext*) {
    ttMsg((TIMERMSG)0);
}
