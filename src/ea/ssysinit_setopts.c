// A fragment of ssysinit.c (0x801528c8): SNDSYS_setopts copies the caller's
// sound system options (the 176-byte block at +32 of its argument) into the
// global state member by member, applies the output settings and then copies
// the first 152 bytes of them on to the applied-options block (+208). sndgs's
// layout is not known: the options block and its members (ints, shorts,
// bytes, a 7-byte name copied as a structure and 8-byte members of two
// words) are inferred from the copies, and the second copy reaches the name
// through a byte offset from the options block, which is how the original
// addresses it. SNDSYS_getopts before it and SNDSYSI_init after it are not
// reconstructed; the rest of the file is in ssysinit.c.
extern "C" char sndgs[];

void SNDPLATFORM_outputset(void);

/* inferred: an 8-byte member (two words) */
struct SNDPAIR {
    int a[2];
};

/* inferred: a 7-byte name */
struct SNDNAME7 {
    char c[7];
};

/* inferred: the sound system options block (176 bytes) */
struct SNDSYSOPTS {
    int field00;
    int field04;
    unsigned short field08;
    unsigned short field0a;
    unsigned short field0c;
    unsigned char field0e;
    unsigned char field0f;
    unsigned char field10;
    unsigned char field11;
    unsigned char field12;
    unsigned char field13;
    unsigned char field14;
    SNDNAME7 name;
    unsigned char field1c;
    unsigned char field1d;
    unsigned char field1e;
    unsigned char field1f;
    unsigned char field20;
    unsigned char field21;
    unsigned char field22;
    unsigned char field23;
    unsigned char field24;
    unsigned char field25;
    unsigned char field26;
    unsigned char field27;
    unsigned char field28;
    unsigned char field29;
    unsigned char field2a;
    unsigned char field2b;
    unsigned char field2c;
    unsigned char field2d;
    unsigned char field2e;
    SNDPAIR field30;
    int field38;
    int field3c;
    int field40;
    int field44;
    int field48;
    int field4c;
    int field50;
    int field54;
    SNDPAIR field58;
    SNDPAIR field60;
    SNDPAIR field68;
    SNDPAIR field70;
    SNDPAIR field78;
    SNDPAIR field80;
    SNDPAIR field88;
    SNDPAIR field90;
    int field98;
    int field9c;
    int fielda0;
    int fielda4;
    int fielda8;
    int fieldac;
};

/* inferred: the options at +32 */
struct SNDSYSOPTSHOLDER {
    unsigned char unknown00[32];
    SNDSYSOPTS opts;
};

/* inferred: the global state's current (+32) and applied (+208) options */
struct SNDGSVIEW {
    unsigned char unknown00[32];
    SNDSYSOPTS opts;
    SNDSYSOPTS applied;
};

extern "C" int SNDSYS_setopts(SNDSYSOPTSHOLDER* holder) {
    ((SNDGSVIEW*)sndgs)->opts.field00 = holder->opts.field00;
    ((SNDGSVIEW*)sndgs)->opts.field04 = holder->opts.field04;
    ((SNDGSVIEW*)sndgs)->opts.field08 = holder->opts.field08;
    ((SNDGSVIEW*)sndgs)->opts.field0a = holder->opts.field0a;
    ((SNDGSVIEW*)sndgs)->opts.field0c = holder->opts.field0c;
    ((SNDGSVIEW*)sndgs)->opts.field0e = holder->opts.field0e;
    ((SNDGSVIEW*)sndgs)->opts.field0f = holder->opts.field0f;
    ((SNDGSVIEW*)sndgs)->opts.field10 = holder->opts.field10;
    ((SNDGSVIEW*)sndgs)->opts.field11 = holder->opts.field11;
    ((SNDGSVIEW*)sndgs)->opts.field12 = holder->opts.field12;
    ((SNDGSVIEW*)sndgs)->opts.field13 = holder->opts.field13;
    ((SNDGSVIEW*)sndgs)->opts.field14 = holder->opts.field14;
    ((SNDGSVIEW*)sndgs)->opts.name = holder->opts.name;
    ((SNDGSVIEW*)sndgs)->opts.field1c = holder->opts.field1c;
    ((SNDGSVIEW*)sndgs)->opts.field1d = holder->opts.field1d;
    ((SNDGSVIEW*)sndgs)->opts.field1e = holder->opts.field1e;
    ((SNDGSVIEW*)sndgs)->opts.field1f = holder->opts.field1f;
    ((SNDGSVIEW*)sndgs)->opts.field20 = holder->opts.field20;
    ((SNDGSVIEW*)sndgs)->opts.field21 = holder->opts.field21;
    ((SNDGSVIEW*)sndgs)->opts.field22 = holder->opts.field22;
    ((SNDGSVIEW*)sndgs)->opts.field23 = holder->opts.field23;
    ((SNDGSVIEW*)sndgs)->opts.field24 = holder->opts.field24;
    ((SNDGSVIEW*)sndgs)->opts.field25 = holder->opts.field25;
    ((SNDGSVIEW*)sndgs)->opts.field26 = holder->opts.field26;
    ((SNDGSVIEW*)sndgs)->opts.field27 = holder->opts.field27;
    ((SNDGSVIEW*)sndgs)->opts.field28 = holder->opts.field28;
    ((SNDGSVIEW*)sndgs)->opts.field29 = holder->opts.field29;
    ((SNDGSVIEW*)sndgs)->opts.field2a = holder->opts.field2a;
    ((SNDGSVIEW*)sndgs)->opts.field2b = holder->opts.field2b;
    ((SNDGSVIEW*)sndgs)->opts.field2c = holder->opts.field2c;
    ((SNDGSVIEW*)sndgs)->opts.field2d = holder->opts.field2d;
    ((SNDGSVIEW*)sndgs)->opts.field2e = holder->opts.field2e;
    ((SNDGSVIEW*)sndgs)->opts.field30 = holder->opts.field30;
    ((SNDGSVIEW*)sndgs)->opts.field38 = holder->opts.field38;
    ((SNDGSVIEW*)sndgs)->opts.field3c = holder->opts.field3c;
    ((SNDGSVIEW*)sndgs)->opts.field40 = holder->opts.field40;
    ((SNDGSVIEW*)sndgs)->opts.field44 = holder->opts.field44;
    ((SNDGSVIEW*)sndgs)->opts.field48 = holder->opts.field48;
    ((SNDGSVIEW*)sndgs)->opts.field4c = holder->opts.field4c;
    ((SNDGSVIEW*)sndgs)->opts.field50 = holder->opts.field50;
    ((SNDGSVIEW*)sndgs)->opts.field54 = holder->opts.field54;
    ((SNDGSVIEW*)sndgs)->opts.field58 = holder->opts.field58;
    ((SNDGSVIEW*)sndgs)->opts.field60 = holder->opts.field60;
    ((SNDGSVIEW*)sndgs)->opts.field68 = holder->opts.field68;
    ((SNDGSVIEW*)sndgs)->opts.field70 = holder->opts.field70;
    ((SNDGSVIEW*)sndgs)->opts.field78 = holder->opts.field78;
    ((SNDGSVIEW*)sndgs)->opts.field80 = holder->opts.field80;
    ((SNDGSVIEW*)sndgs)->opts.field88 = holder->opts.field88;
    ((SNDGSVIEW*)sndgs)->opts.field90 = holder->opts.field90;
    ((SNDGSVIEW*)sndgs)->opts.field98 = holder->opts.field98;
    ((SNDGSVIEW*)sndgs)->opts.field9c = holder->opts.field9c;
    ((SNDGSVIEW*)sndgs)->opts.fielda0 = holder->opts.fielda0;
    ((SNDGSVIEW*)sndgs)->opts.fielda4 = holder->opts.fielda4;
    ((SNDGSVIEW*)sndgs)->opts.fielda8 = holder->opts.fielda8;
    ((SNDGSVIEW*)sndgs)->opts.fieldac = holder->opts.fieldac;
    SNDPLATFORM_outputset();
    ((SNDGSVIEW*)sndgs)->applied.field00 = ((SNDGSVIEW*)sndgs)->opts.field00;
    ((SNDGSVIEW*)sndgs)->applied.field04 = ((SNDGSVIEW*)sndgs)->opts.field04;
    ((SNDGSVIEW*)sndgs)->applied.field08 = ((SNDGSVIEW*)sndgs)->opts.field08;
    ((SNDGSVIEW*)sndgs)->applied.field0a = ((SNDGSVIEW*)sndgs)->opts.field0a;
    ((SNDGSVIEW*)sndgs)->applied.field0c = ((SNDGSVIEW*)sndgs)->opts.field0c;
    ((SNDGSVIEW*)sndgs)->applied.field0e = ((SNDGSVIEW*)sndgs)->opts.field0e;
    ((SNDGSVIEW*)sndgs)->applied.field0f = ((SNDGSVIEW*)sndgs)->opts.field0f;
    ((SNDGSVIEW*)sndgs)->applied.field10 = ((SNDGSVIEW*)sndgs)->opts.field10;
    ((SNDGSVIEW*)sndgs)->applied.field11 = ((SNDGSVIEW*)sndgs)->opts.field11;
    ((SNDGSVIEW*)sndgs)->applied.field12 = ((SNDGSVIEW*)sndgs)->opts.field12;
    ((SNDGSVIEW*)sndgs)->applied.field13 = ((SNDGSVIEW*)sndgs)->opts.field13;
    ((SNDGSVIEW*)sndgs)->applied.field14 = ((SNDGSVIEW*)sndgs)->opts.field14;
    ((SNDGSVIEW*)sndgs)->applied.name = *(SNDNAME7*)((char*)&((SNDGSVIEW*)sndgs)->opts + 21);
    ((SNDGSVIEW*)sndgs)->applied.field1c = ((SNDGSVIEW*)sndgs)->opts.field1c;
    ((SNDGSVIEW*)sndgs)->applied.field1d = ((SNDGSVIEW*)sndgs)->opts.field1d;
    ((SNDGSVIEW*)sndgs)->applied.field1e = ((SNDGSVIEW*)sndgs)->opts.field1e;
    ((SNDGSVIEW*)sndgs)->applied.field1f = ((SNDGSVIEW*)sndgs)->opts.field1f;
    ((SNDGSVIEW*)sndgs)->applied.field20 = ((SNDGSVIEW*)sndgs)->opts.field20;
    ((SNDGSVIEW*)sndgs)->applied.field21 = ((SNDGSVIEW*)sndgs)->opts.field21;
    ((SNDGSVIEW*)sndgs)->applied.field22 = ((SNDGSVIEW*)sndgs)->opts.field22;
    ((SNDGSVIEW*)sndgs)->applied.field23 = ((SNDGSVIEW*)sndgs)->opts.field23;
    ((SNDGSVIEW*)sndgs)->applied.field24 = ((SNDGSVIEW*)sndgs)->opts.field24;
    ((SNDGSVIEW*)sndgs)->applied.field25 = ((SNDGSVIEW*)sndgs)->opts.field25;
    ((SNDGSVIEW*)sndgs)->applied.field26 = ((SNDGSVIEW*)sndgs)->opts.field26;
    ((SNDGSVIEW*)sndgs)->applied.field27 = ((SNDGSVIEW*)sndgs)->opts.field27;
    ((SNDGSVIEW*)sndgs)->applied.field28 = ((SNDGSVIEW*)sndgs)->opts.field28;
    ((SNDGSVIEW*)sndgs)->applied.field29 = ((SNDGSVIEW*)sndgs)->opts.field29;
    ((SNDGSVIEW*)sndgs)->applied.field2a = ((SNDGSVIEW*)sndgs)->opts.field2a;
    ((SNDGSVIEW*)sndgs)->applied.field2b = ((SNDGSVIEW*)sndgs)->opts.field2b;
    ((SNDGSVIEW*)sndgs)->applied.field2c = ((SNDGSVIEW*)sndgs)->opts.field2c;
    ((SNDGSVIEW*)sndgs)->applied.field2d = ((SNDGSVIEW*)sndgs)->opts.field2d;
    ((SNDGSVIEW*)sndgs)->applied.field2e = ((SNDGSVIEW*)sndgs)->opts.field2e;
    ((SNDGSVIEW*)sndgs)->applied.field30 = ((SNDGSVIEW*)sndgs)->opts.field30;
    ((SNDGSVIEW*)sndgs)->applied.field38 = ((SNDGSVIEW*)sndgs)->opts.field38;
    ((SNDGSVIEW*)sndgs)->applied.field3c = ((SNDGSVIEW*)sndgs)->opts.field3c;
    ((SNDGSVIEW*)sndgs)->applied.field40 = ((SNDGSVIEW*)sndgs)->opts.field40;
    ((SNDGSVIEW*)sndgs)->applied.field44 = ((SNDGSVIEW*)sndgs)->opts.field44;
    ((SNDGSVIEW*)sndgs)->applied.field48 = ((SNDGSVIEW*)sndgs)->opts.field48;
    ((SNDGSVIEW*)sndgs)->applied.field4c = ((SNDGSVIEW*)sndgs)->opts.field4c;
    ((SNDGSVIEW*)sndgs)->applied.field50 = ((SNDGSVIEW*)sndgs)->opts.field50;
    ((SNDGSVIEW*)sndgs)->applied.field54 = ((SNDGSVIEW*)sndgs)->opts.field54;
    ((SNDGSVIEW*)sndgs)->applied.field58 = ((SNDGSVIEW*)sndgs)->opts.field58;
    ((SNDGSVIEW*)sndgs)->applied.field60 = ((SNDGSVIEW*)sndgs)->opts.field60;
    ((SNDGSVIEW*)sndgs)->applied.field68 = ((SNDGSVIEW*)sndgs)->opts.field68;
    ((SNDGSVIEW*)sndgs)->applied.field70 = ((SNDGSVIEW*)sndgs)->opts.field70;
    ((SNDGSVIEW*)sndgs)->applied.field78 = ((SNDGSVIEW*)sndgs)->opts.field78;
    ((SNDGSVIEW*)sndgs)->applied.field80 = ((SNDGSVIEW*)sndgs)->opts.field80;
    ((SNDGSVIEW*)sndgs)->applied.field88 = ((SNDGSVIEW*)sndgs)->opts.field88;
    ((SNDGSVIEW*)sndgs)->applied.field90 = ((SNDGSVIEW*)sndgs)->opts.field90;
    return 0;
}
