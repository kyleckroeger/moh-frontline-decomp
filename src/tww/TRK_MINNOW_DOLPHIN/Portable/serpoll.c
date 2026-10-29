#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/serpoll.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/nubevent.h"
#include "trk.h"

static TRKFramingState gTRKFramingState;

void* gTRKInputPendingPtr;

extern int TRKPollUART(void);
void TRKProcessInput(int bufferIdx);

extern int TRKReadUARTPoll(char* c);
extern DSError TRKGetFreeBuffer(MessageBufferID* id, TRKBuffer** buffer);
extern void TRKReleaseBuffer(MessageBufferID id);
extern DSError TRKStandardACK(TRKBuffer* buffer, u8 command, u8 reply);

// Frontline's revision receives HDLC-style frames byte by byte: 0x7E
// delimits a frame, 0x7D escapes the next byte (XOR 0x20). The helpers
// below are inlined; TRKAppendBuffer1_ui8 is msgbuf.c's (as in notify.c).
inline DSError TRKAppendBuffer1_ui8(TRKBuffer* buffer, const u8 data) {
    if (buffer->position >= 0x880) {
        return DS_MessageBufferOverflow;
    }

    buffer->data[buffer->position++] = data;
    buffer->length++;
    return DS_NoError;
}

static inline void TRKDiscardFrame(void) {
    if (gTRKFramingState.msgBufID != -1) {
        TRKReleaseBuffer(gTRKFramingState.msgBufID);
        gTRKFramingState.msgBufID = -1;
    }
    gTRKFramingState.buffer = NULL;
    gTRKFramingState.receiveState = DSRECV_Wait;
}

static inline BOOL TRKCheckFrame(void) {
    TRKBuffer* b = gTRKFramingState.buffer;
    if (b->length < 2) {
        TRKStandardACK(b, DSMSG_ReplyNAK, DSREPLY_PacketSizeError);
        TRKDiscardFrame();
        return FALSE;
    }
    b->position = 0;
    b->length--;
    return TRUE;
}

MessageBufferID TRKTestForPacket(void) {
    int ret = 0;
    int err;
    char c;
    MessageBufferID id;

    err = TRKReadUARTPoll(&c);
    while (err == 0 && ret == 0) {
        if (gTRKFramingState.receiveState != DSRECV_InFrame) {
            gTRKFramingState.isEscape = FALSE;
        }
        switch (gTRKFramingState.receiveState) {
        case DSRECV_Wait:
            if ((int)(u8)c == 0x7E) {
                ret = TRKGetFreeBuffer(&gTRKFramingState.msgBufID, &gTRKFramingState.buffer);
                gTRKFramingState.fcsType = 0;
                gTRKFramingState.receiveState = DSRECV_Found;
            }
            break;
        case DSRECV_Found:
            if ((int)(u8)c == 0x7E) {
                break;
            }
            gTRKFramingState.receiveState = DSRECV_InFrame;
            // fall through
        case DSRECV_InFrame:
            if (c == 0x7E) {
                if (gTRKFramingState.isEscape) {
                    TRKStandardACK(gTRKFramingState.buffer, DSMSG_ReplyNAK, DSREPLY_EscapeError);
                    TRKDiscardFrame();
                    break;
                }
                if (TRKCheckFrame()) {
                    id = gTRKFramingState.msgBufID;
                    gTRKFramingState.msgBufID = -1;
                    gTRKFramingState.buffer = NULL;
                    gTRKFramingState.receiveState = DSRECV_Wait;
                    return id;
                }
                gTRKFramingState.receiveState = DSRECV_Wait;
                break;
            }
            if (gTRKFramingState.isEscape) {
                c ^= 0x20;
                gTRKFramingState.isEscape = FALSE;
            } else if (c == 0x7D) {
                gTRKFramingState.isEscape = TRUE;
                break;
            }
            ret = TRKAppendBuffer1_ui8(gTRKFramingState.buffer, c);
            gTRKFramingState.fcsType += c;
            break;
        case DSRECV_FrameOverflow:
            if ((int)(u8)c == 0x7E) {
                TRKDiscardFrame();
            }
            break;
        }
        err = TRKReadUARTPoll(&c);
    }
    return -1;
}

void TRKGetInput(void) {
    TRKBuffer* msgbuffer;
    int bufID;
    u8 command;

    bufID = TRKTestForPacket();

    if (bufID != -1) {
        msgbuffer = TRKGetBuffer(bufID);
        TRKSetBufferPosition(msgbuffer, 0);
        TRKReadBuffer1_ui8(msgbuffer, &command);
        if (command < 0x80) {
            TRKProcessInput(bufID);
        } else {
            TRKReleaseBuffer(bufID);
        }
    }
}

void TRKProcessInput(int bufferIdx) {
    TRKEvent event;

    TRKConstructEvent(&event, NUBEVENT_Request);
    event.msgBufID = bufferIdx;
    gTRKFramingState.msgBufID = -1;
    TRKPostEvent(&event);
}

DSError TRKInitializeSerialHandler() {
    gTRKFramingState.msgBufID = -1;
    gTRKFramingState.receiveState = DSRECV_Wait;
    gTRKFramingState.isEscape = FALSE;


    return DS_NoError;
}

DSError TRKTerminateSerialHandler(void) {
    return DS_NoError;
}
