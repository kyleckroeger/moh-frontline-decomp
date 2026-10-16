// SNDI_patchtohdr (the whole of spat2hdr.c): converts a tagged patch into a
// sample format, attributes and description. The three records are cleared
// and given their defaults (24000 Hz, one channel, seven-bit samples, flags
// 0x200), then the patch's tags are read: format (rate, channels, bits),
// attribute bytes, four copied data blocks (allocated and copied, with their
// lengths), up to four extra blocks, the description's sample addresses, a
// loop table and per-channel pan offsets. Finally, for each channel from the
// last, the pan comes from the sound globals' speaker table plus its offset
// (no offset in two-channel output on a two-speaker platform), the address
// is rebased on the bank for absolute or unflagged samples, and looped
// samples take their addresses from the loop table. The functions and
// SNDTAGINFO, TAGGEDPATCH, SNDSAMPLEFORMAT, SNDSAMPLEATTR and SNDSAMPLEDESC
// are named by the mangled symbols; the record members, the tag numbers'
// meanings and the sound-globals view (platform mode at +72, pan table at
// +512) are inferred. The offsets' zero initialiser is the file's .rodata.
struct SNDTAGINFO {
    unsigned char* p;
    int tag;
    int value;
    unsigned char* data;
    unsigned int length;
};

struct TAGGEDPATCH;

struct SNDSAMPLEFORMAT {
    unsigned short rate;
    unsigned char channels;
    unsigned char bits;
};

struct SNDSAMPLEATTR {
    unsigned char field00[2];
    unsigned char field02;
    unsigned char field03[2];
    unsigned char field05;
    unsigned char field06;
    unsigned char field07;
    unsigned short flags;
    unsigned char field0a[2];
    unsigned short pan[4];
    void* data[4];
    unsigned int length[4];
    unsigned char* extra[4];
    unsigned int extraLength[4];
};

struct SNDSAMPLEDESC {
    int field00;
    int address[4];
};

/* inferred: the sound globals' platform mode (+72) and speaker pan table (+512) */
struct SNDGSVIEW {
    unsigned char unknown000[72];
    unsigned char mode;
    unsigned char unknown049[439];
    unsigned short pan[5][4];
};

extern "C" {
void* memset(void*, int, unsigned long);
extern SNDGSVIEW sndgs;
void SND_attrsetdef(SNDSAMPLEATTR*);
void SNDI_memcpy(void*, const void*, unsigned int);
}

int SNDI_gettag(SNDTAGINFO*);
void* SNDMEMI_allocz(int);

void SNDI_patchtohdr(void* bank, TAGGEDPATCH* patch, SNDSAMPLEFORMAT* format, SNDSAMPLEATTR* attr,
                     SNDSAMPLEDESC* desc) {
    int offsets[4] = { 0, 0, 0, 0 };
    SNDTAGINFO info;
    unsigned int flags = 512;
    int* loop = 0;
    int extra = 0;
    int i;
    memset(format, 0, sizeof(SNDSAMPLEFORMAT));
    memset(attr, 0, sizeof(SNDSAMPLEATTR));
    memset(desc, 0, sizeof(SNDSAMPLEDESC));
    SND_attrsetdef(attr);
    info.p = (unsigned char*)patch + 4;
    format->rate = 24000;
    format->channels = 1;
    format->bits = 7;
    desc->field00 = 0;
    while (SNDI_gettag(&info)) {
        if (info.tag == 160)
            format->bits = info.value;
        else if (info.tag == 156)
            offsets[0] = info.value;
        else if (info.tag == 157)
            offsets[1] = info.value;
        else if (info.tag == 158)
            offsets[2] = info.value;
        else if (info.tag == 159)
            offsets[3] = info.value;
        else if (info.tag == 152) {
            attr->data[0] = SNDMEMI_allocz(info.length);
            SNDI_memcpy(attr->data[0], info.data, info.length);
            attr->length[0] = info.length;
        } else if (info.tag == 153) {
            attr->data[1] = SNDMEMI_allocz(info.length);
            SNDI_memcpy(attr->data[1], info.data, info.length);
            attr->length[1] = info.length;
        } else if (info.tag == 154) {
            attr->data[2] = SNDMEMI_allocz(info.length);
            SNDI_memcpy(attr->data[2], info.data, info.length);
            attr->length[2] = info.length;
        } else if (info.tag == 155) {
            attr->data[3] = SNDMEMI_allocz(info.length);
            SNDI_memcpy(attr->data[3], info.data, info.length);
            attr->length[3] = info.length;
        } else if (info.tag == 128)
            attr->field07 = info.value;
        else if (info.tag == 130)
            format->channels = info.value;
        else if (info.tag == 132)
            format->rate = info.value;
        else if (info.tag == 133)
            desc->field00 = info.value;
        else if (info.tag == 138)
            loop = (int*)info.data;
        else if (info.tag == 19)
            attr->field05 = info.value;
        else if (info.tag == 10)
            attr->field06 = info.value;
        else if (info.tag == 6)
            attr->field02 = info.value;
        else if (info.tag == 140)
            flags = info.value;
        else if (info.tag == 136)
            desc->address[0] = info.value;
        else if (info.tag == 137)
            desc->address[1] = info.value;
        else if (info.tag == 148)
            desc->address[2] = info.value;
        else if (info.tag == 149)
            desc->address[3] = info.value;
        else if (info.tag == 20) {
            attr->extra[extra] = info.data;
            attr->extraLength[extra] = info.length;
            extra++;
        }
    }
    attr->flags = flags;
    for (i = format->channels - 1; i >= 0; i--) {
        if (sndgs.mode == 2 && format->channels == 2)
            offsets[i] = 0;
        attr->pan[i] = sndgs.pan[format->channels][i] + offsets[i];
        if ((attr->flags & 20) || !attr->flags)
            desc->address[i] = (int)bank + desc->address[i];
        if ((attr->flags & 776) && loop) {
            if (i == 0)
                desc->address[i] = loop[0];
            else
                desc->address[i] = loop[0] + (desc->address[i] - desc->address[0]);
        }
    }
}
