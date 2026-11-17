// REAL mutexes on OS mutexes; destroying one poisons its first word. The
// record's layout is inferred from offsets. OSMutex is the SDK's (dolphin/os.h).
#include <dolphin/os.h>

struct MUTEX {
    int field0;
    OSMutex mutex;
};

extern "C" {
void MEM_fill(void*, int, int);

int MUTEX_create(MUTEX* mutex) {
    OSInitMutex(&mutex->mutex);
    return 1;
}

void MUTEX_destroy(MUTEX* mutex) {
    MEM_fill(mutex, 0xdeadbeef, 4);
}

void MUTEX_lock(MUTEX* mutex) {
    OSLockMutex(&mutex->mutex);
}

void MUTEX_unlock(MUTEX* mutex) {
    OSUnlockMutex(&mutex->mutex);
}
}
