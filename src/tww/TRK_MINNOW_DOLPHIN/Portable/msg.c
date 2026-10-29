/**
 * msg.c
 * Description:
 */

#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msg.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk_glue.h"
#include "trk.h"

/* Frontline's TRKMessageSend frames the message itself over the UART layer
   (neither TWW's nor Pikmin's version, which hand the buffer to
   TRKWriteUARTN): a one's-complement byte checksum, 0x7E delimiters and 0x7D
   escapes (the escaped byte is XORed with 0x20). Written from the target.
   The checksum byte is sent by a one-pass copy of the escape loop (the target
   keeps that loop's dead final error compare). WriteUART1 and WriteUARTFlush
   are the UART layer's functions; the target sign-extends the byte it passes
   to WriteUART1, so it is declared with an s8 parameter here. */
int WriteUART1(s8 c);
int WriteUARTFlush(void);

DSError TRKMessageSend(TRKBuffer* buffer) {
    u8 sum = 0;
    u8 fcs;
    u32 i;
    int err;

    {
        int j;
        for (j = 0; j < buffer->length; j++)
            sum = (u8)(sum + buffer->data[j]);
    }
    fcs = sum ^ 0xFF;

    err = WriteUART1(0x7E);
    if (err == 0) {
        for (i = 0; i < buffer->length; i++) {
            u8 c = buffer->data[i];
            if (c == 0x7E || c == 0x7D) {
                err = WriteUART1(0x7D);
                c ^= 0x20;
                if (err != 0)
                    break;
            }
            err = WriteUART1(c);
            if (err != 0)
                break;
        }
    }
    if (err == 0) {
        u8 c = fcs;
        for (i = 0; i < 1; i++) {
            if (c == 0x7E || c == 0x7D) {
                err = WriteUART1(0x7D);
                c ^= 0x20;
                if (err != 0)
                    break;
            }
            err = WriteUART1(c);
            if (err != 0)
                break;
        }
    }
    if (err == 0)
        err = WriteUART1(0x7E);
    if (err == 0)
        err = WriteUARTFlush();
    return err;
}
