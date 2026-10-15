// A fragment of Moh2.cpp (0x80017e2c): GetGameTime converts the frame count
// to seconds at 59.94 frames a second; GetFrameCount returns the count. The function is named by the symbol;
// g_frameNum is file-local in the original and declared extern here. The rest
// of the file is in other units.
extern int g_frameNum; // file-local in the original

float GetGameTime() {
    return 0.0166833512f * g_frameNum;
}

int GetFrameCount() {
    return g_frameNum;
}
