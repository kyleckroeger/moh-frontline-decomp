// User-data clients of the sound system: a callback list in sndgs; and
// SNDI_validrendermode, which advances an index over the system's render
// modes to the next one compatible with a patch's wanted mode flags (the
// 0x71c and 0xe0 groups, and flag 0x80 only for headers whose byte at +3 is
// 1). sndgs's layout is not known; the members used here are an inferred
// view. The first group's else branch re-assigns the value already held: the
// original keeps that (empty) block, giving its 'bne; b' branch pair.
struct SNDGSVIEW {
    char field0[77];
    unsigned char rendermodes;
    char field4E[2];
    unsigned short rendermode[140];
    signed char running;
    char field169[5];
    signed char userdataclients;
    char field16F[57];
    void* userdataclient[1];
};

extern "C" {
extern SNDGSVIEW sndgs;
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);

void SNDSYS_adduserdataclient(void* client) {
    if (sndgs.running) {
        sndgs.userdataclient[sndgs.userdataclients] = client;
        sndgs.userdataclients++;
    }
}

void SNDSYS_removeuserdataclient(void* client) {
    int i;

    SNDSYS_entercritical();
    for (i = 0; i < sndgs.userdataclients; i++) {
        if (sndgs.userdataclient[i] == client) {
            sndgs.userdataclients--;
            for (; i < sndgs.userdataclients; i++)
                sndgs.userdataclient[i] = sndgs.userdataclient[i + 1];
            break;
        }
    }
    SNDSYS_leavecritical();
}
}

// Inferred: the patch header's render-mode flags (+24) and a byte at +3 that
// must be 1 for modes with flag 0x80.
struct SNDIPATCHHEADER {
    unsigned char unknown00[3];
    signed char unknown03;
    unsigned char unknown04[20];
    unsigned short rendermode;
};

int SNDI_validrendermode(int* index, SNDIPATCHHEADER* header) {
    int valid = 0;

    for (; *index < sndgs.rendermodes; (*index)++) {
        unsigned short mode = sndgs.rendermode[*index];
        unsigned short wanted = header->rendermode;
        int other;

        valid = mode & 0x71c;
        if (!(wanted & 0x71c)) {
            valid = mode & 0x71c;
        } else {
            int both = wanted & valid;
            valid = both;
            if (!both) {
                valid = 0;
                continue;
            }
        }
        other = mode & 0xe0;
        if (!(wanted & 0xe0)) {
            valid |= other;
        } else {
            valid |= wanted & other;
            if (!(wanted & other)) {
                valid = 0;
                continue;
            }
        }
        if (header->unknown03 != 1 && (valid & 0x80)) {
            valid = 0;
            continue;
        }
        if (valid) {
            (*index)++;
            break;
        }
    }
    return valid;
}
