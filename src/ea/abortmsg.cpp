// REAL abort messages: format the message, then hand it to the abort hook
// (REAL_abortmessage) or print it with the recorded file and line and exit
// (SYSTEM_abortmessage).
#include "game/GameVarargs.h"

extern "C" {
void PRINT_string(int, const char*, ...);
void REAL_exit(void);
extern void (*REALabortmessage)(const char*, ...);

char* REALabortfilename;
int REALabortlinenum;

void REAL_abortmessage(const char* format, ...) {
    std::va_list args;
    char buffer[512];

    if (format) {
        GAME_VA_START(args, format);
        vsprintf(buffer, format, args);
        GAME_VA_END(args);
    } else {
        buffer[0] = 0;
    }
    REALabortmessage("%s", buffer);
}

void SYSTEM_abortmessage(const char* format, ...) {
    std::va_list args;
    char buffer[512];

    if (format) {
        GAME_VA_START(args, format);
        vsprintf(buffer, format, args);
        GAME_VA_END(args);
    } else {
        buffer[0] = 0;
    }
    PRINT_string(2, "ERROR: %s", buffer);
    if (REALabortfilename)
        PRINT_string(2, "FILE %s LINE %d\n", REALabortfilename, REALabortlinenum);
    REAL_exit();
}
}
