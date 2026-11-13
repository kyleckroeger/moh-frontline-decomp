// REAL thread helpers: record the main thread, the alarm handler that wakes a
// sleeping thread, THREAD_yield (yield when the delay is zero ticks, else sleep
// on a signal until an alarm fires) and THREAD_iscurrent (compare the current
// thread with the given one: null means the main thread, -1 the current one).
// The alarm and its signal are kept together so the handler can find the
// signal; that record, the OS structures and the tick conversion inline are
// inferred.
struct OSAlarm {
    int data[10];
};

struct OSContext;
struct OSThread;

struct OSMessageQueue {
    int data[8];
};

struct SIGNAL {
    int field0;
    OSMessageQueue queue;
    void* message[1];
};

struct THREADALARM {
    OSAlarm alarm;
    SIGNAL signal;
};

extern "C" {
OSThread* OSGetCurrentThread(void);
void SIGNAL_set(SIGNAL*);
void SIGNAL_create(SIGNAL*);
void SIGNAL_wait(SIGNAL*);
void OSYieldThread(void);
void OSCreateAlarm(OSAlarm*);
void OSSetAlarm(OSAlarm*, long long, void (*)(OSAlarm*, OSContext*));
void OSCancelAlarm(OSAlarm*);
}
void InitAlarm();

OSThread* g_thMain;

extern "C" void THREAD_init(void) {
    InitAlarm();
    g_thMain = OSGetCurrentThread();
}

static void AlarmHandler(OSAlarm* alarm, OSContext*) {
    SIGNAL_set(&((THREADALARM*)alarm)->signal);
}

/* inferred: milliseconds to OS timer ticks (the bus clock at 0x800000F8,
   four bus cycles per tick) */
static inline long long mstoticks(int ms) {
    return (long long)ms * (int)((*(unsigned int*)0x800000F8 / 4) / 1000);
}

extern "C" void THREAD_yield(int ms) {
    THREADALARM sleep;
    long long ticks = mstoticks(ms);

    if (!ticks) {
        OSYieldThread();
        return;
    }
    SIGNAL_create(&sleep.signal);
    OSCreateAlarm(&sleep.alarm);
    OSSetAlarm(&sleep.alarm, ticks, AlarmHandler);
    SIGNAL_wait(&sleep.signal);
    OSCancelAlarm(&sleep.alarm);
}

extern "C" bool THREAD_iscurrent(OSThread* thread) {
    OSThread* current = OSGetCurrentThread();

    return current == (thread == 0 ? g_thMain : thread == (OSThread*)-1 ? OSGetCurrentThread() : thread);
}
