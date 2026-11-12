// saemsamb.c: its initialised data (.sdata): SNDAEMS_addmodulebank, set to this file's
// SNDAEMSI_addmodulebank (src/ea/saemsamb.c). The file is inferred as for saems.c's hooks.
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/saemsamb.json


int SNDAEMSI_addmodulebank(void* data, char* name, int flag, void* (*realloc)(void*, int, int));

int (*SNDAEMS_addmodulebank)(void*, char*, int, void* (*)(void*, int, int)) = SNDAEMSI_addmodulebank;
