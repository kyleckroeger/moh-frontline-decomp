/* PATHI_sortprojects (path.c, 0x8010fd80, the file's last function): sorts
   the four project states by id, ascending, swapping whole states through a
   temporary (the compiler copies a state member by member). The function and
   pfstates are named by their symbols; the state layout (424 bytes, as in
   path_service.c with the members the copy reveals: seven words, a byte, a
   3-byte array, ten words, a 64-word block and the 24 track pointers) and the
   loop shape are inferred. The rest of the file is in other units. */
struct PATHSTATEVIEW {
    int leader;
    unsigned int latency;
    int beat;
    int field0c;
    int field10;
    int field14;
    int field18;
    signed char field1c;
    char field1d[3];
    unsigned int id;
    void* active;
    int field28;
    int field2c;
    int field30;
    int field34;
    int field38;
    int field3c;
    int field40;
    int field44;
    int field48[64];
    void* tracks[24];
};

extern PATHSTATEVIEW pfstates[4];

void PATHI_sortprojects() {
    int i;
    int j;
    PATHSTATEVIEW temp;

    for (i = 0; i < 4; i++) {
        for (j = i + 1; j < 4; j++) {
            if (pfstates[i].id > pfstates[j].id) {
                temp = pfstates[i];
                pfstates[i] = pfstates[j];
                pfstates[j] = temp;
            }
        }
    }
}
