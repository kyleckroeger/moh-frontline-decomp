// SNDtimeremaining: the longest time remaining (hundredths of a second)
// among the platform voices playing a sound, 0x7fffffff for a voice whose
// pitch field is 16 or less, -7 when no voice plays it. sndgs's layout is not
// known; the voice table is accessed through inferred offsets.
extern "C" char sndgs[];
int SNDVOICEI_get(int);
int iSNDpatchkey(int, int*);
int SNDPLATFORM_getcurframe(int);

extern "C" int SNDtimeremaining(int handle) {
    int result;
    int voice;
    int key;
    result = -7;
    voice = SNDVOICEI_get(handle);
    if (voice >= 0) {
        key = -1;
        while (iSNDpatchkey(voice, &key)) {
            char* record = *(char**)(sndgs + 468) + key * 128;
            int frame;

            if (*(int*)(record + 20) > 0)
                frame = -7;
            else
                frame = SNDPLATFORM_getcurframe(key);
            if (frame >= 0) {
                int time = (*(int*)(record + 16) - frame) * 100 / *(unsigned short*)(record + 28);

                if (*(unsigned short*)(record + 126) > 16)
                    time = ((time << 12) / *(unsigned short*)(record + 126) << 12) / *(unsigned short*)(record + 94);
                else
                    time = 0x7fffffff;
                if (time > result)
                    result = time;
            }
        }
    }
    return result;
}
