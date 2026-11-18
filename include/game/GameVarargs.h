// Variadic functions in game and EA code. On the GameCube they use MSL's va_list
// (std::__tag_va_List, which the mangled symbols name) and CodeWarrior's
// __builtin_va_info, which is what MSL's va_start expands to; va_end does nothing.
// Where GAME_HOST_LIBC is defined (the PC port, whose C library is the host's),
// both come from <cstdarg> and vsprintf from <cstdio>. See research/notes/sdk-views.md.
#ifndef GAME_GAMEVARARGS_H
#define GAME_GAMEVARARGS_H

#ifdef GAME_HOST_LIBC
#include <cstdarg>
#include <cstdio>
#define GAME_VA_START(ap, last) va_start(ap, last)
#define GAME_VA_END(ap) va_end(ap)
#else
namespace std {
struct __tag_va_List {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
};
typedef __tag_va_List va_list[1];
}

extern "C" int vsprintf(char*, const char*, std::va_list);

#define GAME_VA_START(ap, last) __builtin_va_info(&ap)
#define GAME_VA_END(ap)
#endif

#endif
