#include "direct_io.h"
#include "critical_regions.h"
#include "misc_io.h"
#include "string.h"
#include "wchar_io.h"
#include "FILE_POS.h"
#include "buffer_io.h"

// Frontline also links fread and __fread (not in the upstream file),
// reconstructed from the disassembly. They follow fwrite and __fwrite in the
// target because -inline deferred emits functions in reverse source order.
int __load_buffer(FILE* file, size_t* bytes_loaded, int alignment);
int __flush_line_buffered_output_files(void);

#define set_eof(file)                                                                              \
    do {                                                                                           \
        (file)->file_state.io_state = __neutral;                                                   \
        (file)->file_state.eof = 1;                                                                \
        (file)->buffer_length = 0;                                                                 \
    } while (0)

size_t __fread(void* buffer, size_t size, size_t count, FILE* stream);

size_t fread(void* buffer, size_t size, size_t count, FILE* stream) {
    size_t retval;
    retval = __fread(buffer, size, count, stream);
    return (retval);
}

size_t __fread(void* buffer, size_t size, size_t count, FILE* stream) {
    unsigned char* read_ptr;
    size_t num_bytes, bytes_to_go, bytes_read;
    int ioresult, always_buffer;

#ifndef __NO_WIDE_CHAR
    if (fwide(stream, 0) == 0)
        fwide(stream, -1);
#endif

    bytes_to_go = size * count;

    if (!bytes_to_go || stream->file_state.error || stream->file_mode.file_kind == __closed_file)
        return 0;

    always_buffer = !stream->file_mode.binary_io || stream->file_mode.buffer_mode == _IOFBF;

    if (stream->file_state.io_state == __neutral) {
        if (stream->file_mode.io_mode & __read) {
            stream->file_state.io_state = __reading;
            stream->buffer_length = 0;
        }
    }

    if (stream->file_state.io_state < __reading) {
        set_error(stream);
        return 0;
    }

    if (stream->file_mode.buffer_mode & _IOLBF) {
        if (__flush_line_buffered_output_files()) {
            set_error(stream);
            return 0;
        }
    }

    read_ptr = (unsigned char*)buffer;
    bytes_read = 0;

    if (bytes_to_go && stream->file_state.io_state >= __rereading) {
        do {
#ifndef __NO_WIDE_CHAR
            if (fwide(stream, 0) == 1) {
                *(wchar_t*)read_ptr = stream->ungetc_wide_buffer[stream->file_state.io_state - __rereading];
                read_ptr += sizeof(wchar_t);
                bytes_read += sizeof(wchar_t);
                bytes_to_go -= sizeof(wchar_t);
            } else
#endif
            {
                *read_ptr++ = stream->ungetc_buffer[stream->file_state.io_state - __rereading];
                bytes_read++;
                bytes_to_go--;
            }
            stream->file_state.io_state--;
        } while (bytes_to_go && stream->file_state.io_state >= __rereading);

        if (stream->file_state.io_state == __reading)
            stream->buffer_length = stream->save_buffer_length;
    }

    if (bytes_to_go && (stream->buffer_length || always_buffer)) {
        do {
            if (!stream->buffer_length) {
                ioresult = __load_buffer(stream, NULL, __align_buffer);

                if (ioresult) {
                    if (ioresult == __io_error)
                        set_error(stream);
                    else
                        set_eof(stream);

                    bytes_to_go = 0;
                    break;
                }
            }

            num_bytes = stream->buffer_length;

            if (num_bytes > bytes_to_go)
                num_bytes = bytes_to_go;

            memcpy(read_ptr, stream->buffer_ptr, num_bytes);

            read_ptr += num_bytes;
            bytes_read += num_bytes;
            bytes_to_go -= num_bytes;

            stream->buffer_ptr += num_bytes;
            stream->buffer_length -= num_bytes;
        } while (bytes_to_go && always_buffer);
    }

    if (bytes_to_go && !always_buffer) {
        unsigned char* save_buffer = stream->buffer;
        size_t save_size = stream->buffer_size;

        stream->buffer = read_ptr;
        stream->buffer_size = bytes_to_go;

        ioresult = __load_buffer(stream, &num_bytes, __dont_align_buffer);

        if (ioresult) {
            if (ioresult == __io_error)
                set_error(stream);
            else
                set_eof(stream);
        }

        bytes_read += num_bytes;

        stream->buffer = save_buffer;
        stream->buffer_size = save_size;

        __prep_buffer(stream);

        stream->buffer_length = 0;
    }

    return (bytes_read / size);
}

size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream) {
    size_t retval;
    retval = __fwrite(buffer, size, count, stream);
    return (retval);
}

size_t __fwrite(const void* buffer, size_t size, size_t count, FILE* stream) {
    unsigned char* write_ptr;
    size_t num_bytes, bytes_to_go, bytes_written;
    int ioresult, always_buffer;

#ifndef __NO_WIDE_CHAR
    if (fwide(stream, 0) == 0)
        fwide(stream, -1);
#endif

    bytes_to_go = size * count;

    if (!bytes_to_go || stream->file_state.error || stream->file_mode.file_kind == __closed_file)
        return 0;

    if (stream->file_mode.file_kind == __console_file)
        __stdio_atexit();

    always_buffer = !stream->file_mode.binary_io || stream->file_mode.buffer_mode == _IOFBF ||
                    stream->file_mode.buffer_mode == _IOLBF;

    if (stream->file_state.io_state == __neutral) {
        if (stream->file_mode.io_mode & __write) {
            if (stream->file_mode.io_mode & __append) {
                if (fseek(stream, 0, SEEK_END))
                    return 0;
            }
            stream->file_state.io_state = __writing;

            __prep_buffer(stream);
        }
    }

    if (stream->file_state.io_state != __writing) {
        set_error(stream);
        return 0;
    }

    write_ptr = (unsigned char*)buffer;
    bytes_written = 0;

    if (bytes_to_go && (stream->buffer_ptr != stream->buffer || always_buffer)) {
        stream->buffer_length = stream->buffer_size - (stream->buffer_ptr - stream->buffer);

        do {
            unsigned char* newline = NULL;

            num_bytes = stream->buffer_length;

            if (num_bytes > bytes_to_go)
                num_bytes = bytes_to_go;
            if (stream->file_mode.buffer_mode == _IOLBF && num_bytes)
                if ((newline = (unsigned char*)__memrchr(write_ptr, '\n', num_bytes)) != NULL)
                    num_bytes = newline + 1 - write_ptr;

            if (num_bytes) {
                memcpy(stream->buffer_ptr, write_ptr, num_bytes);

                write_ptr += num_bytes;
                bytes_written += num_bytes;
                bytes_to_go -= num_bytes;

                stream->buffer_ptr += num_bytes;
                stream->buffer_length -= num_bytes;
            }
            if (!stream->buffer_length || newline != NULL ||
                (stream->file_mode.buffer_mode == _IONBF))
            {
                ioresult = __flush_buffer(stream, NULL);

                if (ioresult) {
                    set_error(stream);
                    bytes_to_go = 0;
                    break;
                }
            }
        } while (bytes_to_go && always_buffer);
    }

    if (bytes_to_go && !always_buffer) {
        unsigned char* save_buffer = stream->buffer;
        size_t save_size = stream->buffer_size;

        stream->buffer = write_ptr;
        stream->buffer_size = bytes_to_go;
        stream->buffer_ptr = write_ptr + bytes_to_go;

        if (__flush_buffer(stream, &num_bytes) != __no_io_error)
            set_error(stream);

        bytes_written += num_bytes;

        stream->buffer = save_buffer;
        stream->buffer_size = save_size;

        __prep_buffer(stream);

        stream->buffer_length = 0;
    }

    if (stream->file_mode.buffer_mode != _IOFBF)
        stream->buffer_length = 0;

    return ((bytes_written + size - 1) / size);
}
