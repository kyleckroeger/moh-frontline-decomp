/* Frontline's file-support requests, the functions at the start of the
   file record. TRKRequestSend follows Pikmin's CC0 support.c
   (https://github.com/doldecomp/pikmin) with its two reply bytes declared in
   the other order; the one-byte append is inlined (local inline copy of
   msgbuf.c's TRKAppendBuffer1_ui8). The open, close and position requests are
   Frontline's older protocol, written from the target: the command and
   arguments are appended one by one (no CommandReply header), the I/O result
   is a byte, and the reply is read from position 2. Functions are emitted in
   reverse source order (-inline deferred), so TRKSuppAccessFile, the last
   function in the target, comes first. */
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/support.h"
#include "TRK_MINNOW_DOLPHIN/utils/common/MWTrace.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"
#include "string.h"

/* The append is inlined, as in Pikmin's trk.h (msgbuf.c's body). */
inline DSError TRKAppendBuffer1_ui8(TRKBuffer* buffer, const u8 data) {
    if (buffer->position >= 0x880) {
        return DS_MessageBufferOverflow;
    }

    buffer->data[buffer->position++] = data;
    buffer->length++;
    return DS_NoError;
}


/* Pikmin's support.c body and local order; 'done' declared after 'error'
   gives the original's registers (declared first, it trades r29/r30 with the
   read flag). */
DSError TRKSuppAccessFile(u32 file_handle, u8* data, size_t* count, u8* io_result, BOOL need_reply, BOOL read)
{
	TRKBuffer* replyBuffer;
	int replyBufferId;
	u32 length;
	int bufferId;
	TRKBuffer* buffer;
	DSError error;
	u32 done;
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

DSError TRKRequestSend(TRKBuffer* msgBuf, int* bufferId, u32 p1, u32 p2, int p3)
{
	int error = DS_NoError;
	TRKBuffer* buffer;
	u32 timer;
	int tries;
	u8 msg_command;
	u8 msg_error;
	BOOL badReply = TRUE;

	*bufferId = -1;

	for (tries = p2 + 1; tries != 0 && *bufferId == -1 && error == DS_NoError; tries--) {
		error = TRKMessageSend(msgBuf);
		if (error == DS_NoError) {
			if (p3) {
				timer = 0;
			}

			while (TRUE) {
				do {
					*bufferId = TRKTestForPacket();
					if (*bufferId != -1)
						break;
				} while (!p3 || ++timer < 79999980);

				if (*bufferId == -1)
					break;

				badReply = FALSE;

				buffer = TRKGetBuffer(*bufferId);
				TRKSetBufferPosition(buffer, 0);

				if ((error = TRKReadBuffer1_ui8(buffer, &msg_command)) != DS_NoError)
					break;

				if (msg_command >= DSMSG_ReplyACK)
					break;

				TRKProcessInput(*bufferId);
				*bufferId = -1;
			}

			if (*bufferId != -1) {
				if (buffer->length < p1) {
					badReply = TRUE;
				}
				if (error == DS_NoError && !badReply) {
					error = TRKReadBuffer1_ui8(buffer, &msg_error);
				}
				if (error == DS_NoError && !badReply) {
					if (msg_command != DSMSG_ReplyACK || msg_error != DSREPLY_NoError) {
						badReply = TRUE;
					}
				}
				if (error != DS_NoError || badReply) {
					TRKReleaseBuffer(*bufferId);
					*bufferId = -1;
				}
			}
		}
	}

	if (*bufferId == -1) {
		error = DS_Error800;
	}

	return error;
}

DSError HandleOpenFileSupportRequest(const char* path, u8 mode, u32* handle, u8* ioResult)
{
	DSError error;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	TRKBuffer* replyBuffer;

	*handle = 0;
	error = TRKGetFreeBuffer(&bufferId, &buffer);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, DSMSG_OpenFile);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, mode);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui16(buffer, strlen(path) + 1);
	if (error == DS_NoError)
		error = TRKAppendBuffer_ui8(buffer, (u8*)path, strlen(path) + 1);
	if (error == DS_NoError) {
		*ioResult = 0;
		error = TRKRequestSend(buffer, &replyBufferId, 7, 3, 0);
		if (error == DS_NoError) {
			replyBuffer = TRKGetBuffer(replyBufferId);
			TRKSetBufferPosition(replyBuffer, 2);
		}
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui32(replyBuffer, handle);
		TRKReleaseBuffer(replyBufferId);
	}
	TRKReleaseBuffer(bufferId);
	return error;
}

DSError HandleCloseFileSupportRequest(u32 handle, u8* ioResult)
{
	DSError error;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	TRKBuffer* replyBuffer;

	error = TRKGetFreeBuffer(&bufferId, &buffer);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, DSMSG_CloseFile);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui32(buffer, handle);
	if (error == DS_NoError) {
		*ioResult = 0;
		error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
		if (error == DS_NoError) {
			replyBuffer = TRKGetBuffer(replyBufferId);
			TRKSetBufferPosition(replyBuffer, 2);
		}
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
		TRKReleaseBuffer(replyBufferId);
	}
	TRKReleaseBuffer(bufferId);
	return error;
}

DSError HandlePositionFileSupportRequest(DSFileHandle handle, u32* position, u8 mode, u8* ioResult)
{
	DSError error;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	TRKBuffer* replyBuffer;

	error = TRKGetFreeBuffer(&bufferId, &buffer);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, DSMSG_PositionFile);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui32(buffer, handle);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui32(buffer, *position);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, mode);
	if (error == DS_NoError) {
		*ioResult = 0;
		error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
		if (error == DS_NoError) {
			replyBuffer = TRKGetBuffer(replyBufferId);
			TRKSetBufferPosition(replyBuffer, 2);
		}
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui32(replyBuffer, position);
		else
			*position = -1;
		TRKReleaseBuffer(replyBufferId);
	}
	TRKReleaseBuffer(bufferId);
	return error;
}
