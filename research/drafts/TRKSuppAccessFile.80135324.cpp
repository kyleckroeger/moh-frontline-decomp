// support.c (the MetroTRK debugger): reads or writes a host file in blocks of up to
// 0x800 bytes, as partial source. The buffer struct is inferred.
extern "C" {
struct MessageBuffer {
    unsigned char unknown00[8];
    unsigned int m_length;
    unsigned int m_position;
    unsigned char m_data[0x880];
};
int TRKGetFreeBuffer(int*, MessageBuffer**);
int TRKAppendBuffer1_ui32(MessageBuffer*, unsigned int);
int TRKAppendBuffer1_ui16(MessageBuffer*, unsigned short);
int TRKAppendBuffer_ui8(MessageBuffer*, const unsigned char*, int);
int TRKMessageSend(MessageBuffer*);
int TRKRequestSend(MessageBuffer*, int*, int, int, int);
MessageBuffer* TRKGetBuffer(int);
void TRKSetBufferPosition(MessageBuffer*, unsigned int);
int TRKReadBuffer1_ui8(MessageBuffer*, unsigned char*);
int TRKReadBuffer1_ui16(MessageBuffer*, unsigned short*);
int TRKReadBuffer_ui8(MessageBuffer*, unsigned char*, int);
void TRKReleaseBuffer(int);

int TRKSuppAccessFile(unsigned int handle, unsigned char* data, unsigned int* length,
                      unsigned char* ioResult, int needReply, int isRead) {
    if (!data || !*length)
        return 2;
    bool exit = false;
    *ioResult = 0;
    unsigned int done = 0;
    int error = 0;
    MessageBuffer* reply;
    while (!exit && done < *length && !error && !*ioResult) {
        unsigned int block = *length - done;
        if (block > 0x800)
            block = 0x800;
        int bufferId;
        MessageBuffer* buffer;
        error = TRKGetFreeBuffer(&bufferId, &buffer);
        if (!error) {
            unsigned char command = isRead ? 0xd1 : 0xd0;
            if (buffer->m_position < 0x880) {
                error = 0;
                buffer->m_data[buffer->m_position++] = command;
                buffer->m_length++;
            } else {
                error = 0x301;
            }
        }
        if (!error)
            error = TRKAppendBuffer1_ui32(buffer, handle);
        if (!error)
            error = TRKAppendBuffer1_ui16(buffer, block);
        if (!isRead && !error)
            error = TRKAppendBuffer_ui8(buffer, data + done, block);
        if (!error) {
            if (!needReply) {
                error = TRKMessageSend(buffer);
            } else {
                unsigned short replyLength = 0;
                unsigned char replyResult = 0;
                int replyId;
                error = TRKRequestSend(buffer, &replyId, 5, 3, !(isRead && handle == 0));
                if (!error) {
                    reply = TRKGetBuffer(replyId);
                    TRKSetBufferPosition(reply, 2);
                    error = TRKReadBuffer1_ui8(reply, &replyResult);
                }
                if (!error)
                    error = TRKReadBuffer1_ui16(reply, &replyLength);
                if (isRead && !error) {
                    if (reply->m_length != replyLength + 5) {
                        replyLength = reply->m_length - 5;
                        if (!replyResult)
                            replyResult = 1;
                    }
                    if (replyLength <= block)
                        error = TRKReadBuffer_ui8(reply, data + done, replyLength);
                }
                if (replyLength != block) {
                    if ((!isRead || block <= replyLength) && !replyResult)
                        replyResult = 1;
                    exit = true;
                    block = replyLength;
                }
                *ioResult = replyResult;
                TRKReleaseBuffer(replyId);
            }
        }
        TRKReleaseBuffer(bufferId);
        done += block;
    }
    *length = done;
    return error;
}
}
