#include "file_io.h"
#include "buffer_io.h"
#include "ctype.h"

int fclose(FILE* file) {
    int flush_result, close_result;

    if (file == NULL)
        return (-1);
    if (file->file_mode.file_kind == __closed_file)
        return (0);

    flush_result = fflush(file);

    close_result = (*file->close_fn)(file->handle);

    file->file_mode.file_kind = __closed_file;
    file->handle = NULL;

    if (file->file_state.free_buffer)
        free((FILE*)file->buffer);
    return ((flush_result || close_result) ? -1 : 0);
}

int fflush(FILE* file) {
    int pos;

    if (file == NULL) {
        return __flush_all();
    }

    if (file->file_state.error != 0 || file->file_mode.file_kind == __closed_file) {
        return -1;
    }

    if (file->file_mode.io_mode == __read) {
        return 0;
    }

    if (file->file_state.io_state >= __rereading) {
        file->file_state.io_state = __reading;
    }

    if (file->file_state.io_state == __reading) {
        file->buffer_length = 0;
    }

    if (file->file_state.io_state != __writing) {
        file->file_state.io_state = __neutral;
        return 0;
    }

    if (file->file_mode.file_kind != __disk_file) {
        pos = 0;
    } else {
        pos = ftell(file);
    }

    if (__flush_buffer(file, 0) != 0) {
        file->file_state.error = 1;
        file->buffer_length = 0;
        return -1;
    }

    file->file_state.io_state = __neutral;
    file->position = pos;
    file->buffer_length = 0;
    return 0;
}

// Frontline also links fopen and freopen (not in the upstream file),
// reconstructed from the disassembly. They follow fflush because -inline
// deferred emits functions in reverse source order. __get_file_modes, which
// precedes freopen in the target, is defined after it (end of the file).
int fseek(FILE* file, unsigned long offset, int mode);
void clearerr(FILE* stream);
void __stdio_atexit(void);
void __init_file(FILE* file, file_modes mode, char* buff, size_t size);
FILE* __find_unopened_file(void);
int __open_file(const char* name, file_modes mode, __file_handle* handle);

FILE* freopen(const char* name, const char* mode, FILE* file);

FILE* fopen(const char* name, const char* mode) {
    return freopen(name, mode, __find_unopened_file());
}

int __get_file_modes(const char* mode, file_modes* modes);

FILE* freopen(const char* name, const char* mode, FILE* file) {
    file_modes modes;

    __stdio_atexit();

    if (!file)
        return NULL;

    if (file && file->file_mode.file_kind != __closed_file) {
        fflush(file);
        (*file->close_fn)(file->handle);
        file->file_mode.file_kind = __closed_file;
        file->handle = 0;
        if (file->file_state.free_buffer)
            free(file->buffer);
    }

    clearerr(file);

    if (!__get_file_modes(mode, &modes))
        return NULL;

    __init_file(file, modes, 0, 4096);

    if (__open_file(name, modes, &file->handle)) {
        file->file_mode.file_kind = __closed_file;
        if (file->file_state.free_buffer)
            free(file->buffer);
        return NULL;
    }

    if (modes.io_mode & __append)
        fseek(file, 0, 2);

    return file;
}

// Frontline's __get_file_modes, reconstructed from the disassembly (not in
// the upstream file): the mode string's first letter picks the open mode
// (r: must exist, w: create or truncate, a: create if necessary), then 'b'
// and '+' in either order set binary I/O and read-write access. It precedes
// freopen in the target, so it follows it here.
int __get_file_modes(const char* mode, file_modes* modes) {
    const char* mode_ptr = mode;
    unsigned long mode_str;
    unsigned char open_mode;
    unsigned char io_mode;

    modes->file_kind = __disk_file;
    modes->file_orientation = UNORIENTED;
    modes->binary_io = 0;

    mode_str = *mode_ptr++;

    switch (mode_str) {
    case 'r':
        open_mode = 0;
        break;
    case 'w':
        open_mode = 2;
        break;
    case 'a':
        open_mode = 1;
        break;
    default:
        return 0;
    }

    modes->open_mode = open_mode;

    switch (*mode_ptr++) {
    case 'b':
        modes->binary_io = 1;
        if (*mode_ptr == '+')
            mode_str = (mode_str << 8) | '+';
        break;
    case '+':
        mode_str = (mode_str << 8) | '+';
        if (*mode_ptr == 'b')
            modes->binary_io = 1;
        break;
    }

    switch (mode_str) {
    case 'r':
        io_mode = __read;
        break;
    case 'w':
        io_mode = __write;
        break;
    case 'a':
        io_mode = __write | __append;
        break;
    case 'r+':
        io_mode = __read_write;
        break;
    case 'w+':
        io_mode = __read_write;
        break;
    case 'a+':
        io_mode = __read_write | __append;
        break;
    }

    modes->io_mode = io_mode;

    return 1;
}
