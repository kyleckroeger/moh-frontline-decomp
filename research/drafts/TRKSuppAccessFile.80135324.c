// support.c (MetroTRK, Frontline's older file-support protocol): reads or writes a host file in
// blocks of up to 0x800 bytes. Taken from the whole-file attempt scratch/solo/s2/trk/a.c
// (Pikmin/TWW CC0 support.c shape; 'read ? 5 : 5' is the reference source's). Left: the
// read flag and the byte count 'done' take each other's callee registers (r29/r30).
// Unit: the start of src/tww/TRK_MINNOW_DOLPHIN/Portable/support.c (compile as C).
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/support.h"
#include "TRK_MINNOW_DOLPHIN/utils/common/MWTrace.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif
DSError TRKGetFreeBuffer(int*, TRKBuffer**);
void TRKReleaseBuffer(int);
DSError TRKMessageSend(TRKBuffer*);

/* The append is inlined, as in Pikmin's trk.h (msgbuf.c's body). */
inline DSError TRKAppendBuffer1_ui8(TRKBuffer* buffer, const u8 data) {
    if (buffer->position >= 0x880) {
        return DS_MessageBufferOverflow;
    }

    buffer->data[buffer->position++] = data;
    buffer->length++;
    return DS_NoError;
}

DSError TRKSuppAccessFile(u32 file_handle, u8* data, size_t* count, u8* io_result, BOOL need_reply, BOOL read)
{
	u32 done;
	TRKBuffer* replyBuffer;
	int replyBufferId;
	u32 length;
	int bufferId;
	TRKBuffer* buffer;
	DSError error;
	u16 replyLength;
	u8 replyIOResult;
	BOOL exit;

	if (data == NULL || *count == 0) {
		return DS_ParameterError;
	}

	exit       = FALSE;
	*io_result = DS_IONoError;
	done       = 0;
	error      = DS_NoError;
	while (!exit && done < *count && error == DS_NoError && *io_result == DS_IONoError) {
		if (*count - done > 0x800) {
			length = 0x800;
		} else {
			length = *count - done;
		}

		error = TRKGetFreeBuffer(&bufferId, &buffer);

		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui8(buffer, read ? DSMSG_ReadFile : DSMSG_WriteFile);

		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui32(buffer, file_handle);

		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui16(buffer, length);

		if (!read && error == DS_NoError)
			error = TRKAppendBuffer_ui8(buffer, data + done, length);

		if (error == DS_NoError) {
			if (need_reply) {
				replyLength   = 0;
				replyIOResult = 0;

				error = TRKRequestSend(buffer, &replyBufferId, read ? 5 : 5, 3, !(read && file_handle == 0));
				if (error == DS_NoError) {
					replyBuffer = (TRKBuffer*)TRKGetBuffer(replyBufferId);
					TRKSetBufferPosition(replyBuffer, 2);
				}

				if (error == DS_NoError)
					error = TRKReadBuffer1_ui8(replyBuffer, &replyIOResult);

				if (error == DS_NoError)
					error = TRKReadBuffer1_ui16(replyBuffer, &replyLength);

				if (read && error == DS_NoError) {
					if (replyBuffer->length != replyLength + 5) {
						replyLength = replyBuffer->length - 5;
						if (replyIOResult == 0)
							replyIOResult = 1;
					}

					if (replyLength <= length)
						error = TRKReadBuffer_ui8(replyBuffer, data + done, replyLength);
				}

				if (replyLength != length) {
					if ((!read || replyLength >= length) && replyIOResult == 0)
						replyIOResult = 1;
					length = replyLength;
					exit   = TRUE;
				}

				*io_result = (DSIOResult)replyIOResult;
				TRKReleaseBuffer(replyBufferId);
			} else {
				error = TRKMessageSend(buffer);
			}
		}

		TRKReleaseBuffer(bufferId);
		done += length;
	}

	*count = done;
	return error;
}

#ifdef __cplusplus
}
#endif
