// SNDI_parsetimbre (stpparse.c): parses a timbre's tag stream into its patch
// header. Tags 0..37 and 128..167 keep their last value and data pointer (with
// the defaults below); tag 20 also appends a (data, length) pair, tags
// 152..155 set four (data, length) pairs, tag 254 ends the timbre (the
// result is 1). The header is then filled from the tags; the channel
// table at sndgs+512 gives the per-channel offsets. SNDIPATCHHEADER and
// SNDTAGINFO are named by the mangled symbols; the header view and the tag
// meanings are inferred.
struct SNDTAGINFO {
    unsigned char* p;
    int tag;
    int value;
    unsigned char* data;
    unsigned int length;
};

struct SNDIPATCHHEADER {
    short field00;
    unsigned char field02;
    signed char channels;
    short field04;
    short field06;
    unsigned char field08[9];
    unsigned char field11;
    unsigned char field12;
    unsigned char field13;
    unsigned char field14;
    unsigned char field15;
    unsigned char field16;
    signed char count;
    short field18;
    unsigned char field1a;
    unsigned char field1b;
    unsigned char field1c;
    unsigned char field1d;
    unsigned char field1e;
    unsigned char field1f;
    int field20;
    int field24;
    int field28;
    int field2c;
    int field30;
    int data34[4];
    int length44[4];
    short field54;
    short field56;
    int field58;
    int field5c;
    int field60;
    int field64;
    int field68;
    int field6c;
    int field70;
    int field74;
    int field78;
    int data7c[4];
    short length8c[4];
    unsigned short channel94[4];
    int field9c[4];
    int fieldac[4];
    unsigned char* start;
};

extern "C" char sndgs[];
extern int snddefaultenvelope[2];
int SNDI_gettag(SNDTAGINFO*);

int SNDI_parsetimbre(void** stream, SNDIPATCHHEADER* header) {
    int tagvalue[38];
    int tagdata[38];
    int extvalue[40];
    int extdata[40];
    SNDTAGINFO info;
    int chan[4] = {0, 0, 0, 0};
    int i;
    int result;

    info.p = (unsigned char*)*stream;
    header->start = info.p;
    for (i = 0; i < 38; i++)
        tagdata[i] = 0;
    for (i = 0; i < 40; i++)
        extdata[i] = 0;
    for (i = 0; i < 4; i++) {
        header->data7c[i] = 0;
        header->length8c[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        header->field9c[i] = 0;
        header->fieldac[i] = 0;
    }
    tagvalue[1] = 0;
    tagvalue[2] = 127;
    tagvalue[3] = 0;
    tagvalue[4] = 127;
    tagvalue[6] = 0;
    tagvalue[7] = 60;
    tagvalue[8] = -1;
    tagvalue[9] = 1;
    tagvalue[10] = 0;
    tagvalue[12] = 64;
    tagvalue[13] = 0;
    tagvalue[14] = 127;
    tagvalue[15] = 0;
    tagvalue[16] = 0;
    tagvalue[17] = 0;
    tagvalue[18] = 0;
    tagvalue[19] = 0;
    header->count = 0;
    tagvalue[23] = 0;
    tagvalue[25] = (int)snddefaultenvelope;
    tagvalue[26] = -1;
    tagvalue[28] = 127;
    tagvalue[29] = 0;
    tagvalue[30] = 0;
    tagvalue[31] = 0;
    tagvalue[32] = 0;
    tagvalue[33] = 0;
    tagvalue[34] = 0;
    tagvalue[35] = 0;
    tagvalue[36] = 0;
    tagvalue[37] = 1;
    extvalue[0] = 2;
    extvalue[2] = 1;
    extvalue[4] = 24000;
    extvalue[5] = 0;
    extvalue[6] = -1;
    extvalue[7] = -1;
    extvalue[8] = 0;
    extvalue[28] = 0;
    extvalue[9] = 0;
    extvalue[29] = 0;
    extvalue[20] = 0;
    extvalue[30] = 0;
    extvalue[21] = 0;
    extvalue[31] = 0;
    extvalue[32] = 7;
    extvalue[12] = 512;
    result = 0;
    while (SNDI_gettag(&info)) {
        if (info.tag < 38) {
            tagvalue[info.tag] = info.value;
            tagdata[info.tag] = (int)info.data;
            if (info.tag == 20) {
                header->data34[header->count] = (int)info.data;
                header->length44[header->count] = info.length;
                header->count++;
            }
        } else if (info.tag >= 128 && info.tag < 168) {
            extvalue[info.tag - 128] = info.value;
            extdata[info.tag - 128] = (int)info.data;
            if (info.tag == 152) {
                header->data7c[0] = (int)info.data;
                header->length8c[0] = info.length;
            } else if (info.tag == 153) {
                header->data7c[1] = (int)info.data;
                header->length8c[1] = info.length;
            } else if (info.tag == 154) {
                header->data7c[2] = (int)info.data;
                header->length8c[2] = info.length;
            } else if (info.tag == 155) {
                header->data7c[3] = (int)info.data;
                header->length8c[3] = info.length;
            }
        } else if (info.tag == 254) {
            result = 1;
            break;
        }
    }
    header->field08[0] = tagvalue[1];
    header->field08[1] = tagvalue[2];
    header->field08[2] = tagvalue[3];
    header->field08[3] = tagvalue[4];
    header->field08[4] = tagvalue[6];
    header->field08[5] = tagvalue[7];
    header->field08[6] = tagvalue[8];
    header->field08[7] = tagvalue[9];
    header->field08[8] = tagvalue[10];
    header->field11 = tagvalue[12];
    header->field12 = tagvalue[13];
    header->field14 = tagvalue[14];
    header->field15 = tagvalue[15];
    header->field04 = tagvalue[16];
    header->field06 = tagvalue[17];
    header->field20 = tagdata[18] + tagvalue[18];
    header->field16 = tagvalue[19];
    header->field24 = tagdata[23] + tagvalue[23];
    header->field28 = tagdata[25] + tagvalue[25];
    header->field78 = tagvalue[26];
    header->field1a = tagvalue[28];
    header->field2c = tagdata[29] + tagvalue[29];
    header->field1b = tagvalue[30];
    header->field1c = tagvalue[31];
    header->field30 = tagdata[32] + tagvalue[32];
    header->field1d = tagvalue[33];
    header->field54 = tagvalue[34];
    header->field1e = tagvalue[35];
    header->field00 = tagvalue[36];
    header->field1f = tagvalue[37];
    header->field02 = extvalue[0];
    header->channels = extvalue[2];
    header->field13 = extvalue[32];
    header->field56 = extvalue[4];
    header->field58 = extvalue[5];
    header->field5c = extvalue[6];
    header->field60 = extvalue[7];
    header->field18 = extvalue[12];
    header->field64 = extdata[10];
    header->field68 = extvalue[8];
    header->field6c = extvalue[9];
    header->field70 = extvalue[20];
    header->field74 = extvalue[21];
    header->field9c[0] = extdata[15];
    header->field9c[1] = extdata[16];
    chan[0] = extvalue[28];
    chan[1] = extvalue[29];
    chan[2] = extvalue[30];
    chan[3] = extvalue[31];
    if (header->channels == 3)
        header->field9c[2] = extdata[17];
    header->fieldac[0] = extdata[13];
    header->fieldac[1] = extdata[14];
    if (header->channels == 3)
        header->fieldac[2] = extdata[11];
    for (i = 0; i < header->channels; i++) {
        if ((unsigned char)sndgs[72] == 2 && header->channels == 2)
            chan[i] = 0;
        header->channel94[i] = *(unsigned short*)(sndgs + 512 + header->channels * 8 + i * 2) + chan[i];
    }
    *stream = info.p;
    return result;
}
