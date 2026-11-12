// saems.c: its initialised data (.sdata): the AEMS system's entry hooks, each set to this
// file's SNDAEMSI_ function. The file is inferred: no file map range covers them, but they
// point at saems.c's functions (src/ea/saems_system.c) and EA's libraries define each hook
// beside its function (src/ea/saemsmbf.c's SNDAEMS_asyncloadmodulebank). The hooks'
// types are the functions' own. Checked by research/tools/check_data.py.
// settings: config/GMFE69/saems.json


int SNDAEMSI_addsystem(void* data);
int SNDAEMSI_removesystem();
int SNDAEMSI_removemodulebank(int index);
int SNDAEMSI_beginevent(void* data);
int SNDAEMSI_updateevent(int id, void* data);
int SNDAEMSI_endevent(int id, void* data);
int SNDAEMSI_setglobal(int index, int value);

int (*SNDAEMS_addsystem)(void*) = SNDAEMSI_addsystem;
int (*SNDAEMS_removesystem)() = SNDAEMSI_removesystem;
int (*SNDAEMS_removemodulebank)(int) = SNDAEMSI_removemodulebank;
int (*SNDAEMS_beginevent)(void*) = SNDAEMSI_beginevent;
int (*SNDAEMS_updateevent)(int, void*) = SNDAEMSI_updateevent;
int (*SNDAEMS_endevent)(int, void*) = SNDAEMSI_endevent;
int (*SNDAEMS_setglobal)(int, int) = SNDAEMSI_setglobal;
