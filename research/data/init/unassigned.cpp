// unassigned: initialised globals whose original file is not known (no file
// map range covers them, and their neighbours in the original don't settle it). Each
// comment names the neighbours. Checked by research/tools/check_data.py.

// .sdata2, between capsule.cpp's and collision.cpp's constants.
extern const float CONTACT_EPSILON;
extern const float CONTACT_EPSILON_SQRD;
const float CONTACT_EPSILON = 0.015625f;
const float CONTACT_EPSILON_SQRD = 0.000244140625f;

// .sdata2, between jaw_controller.cpp's and player.cpp's constants.
extern const float DEFAULT_FOV;
const float DEFAULT_FOV = 35.0f;

// .sdata2, between weaponfactory.cpp's and scene.cpp's constants.
extern const float TIME_STEP_EPSILON;
const float TIME_STEP_EPSILON = 0.015625f;

// .sdata, after destructor_queue.cpp's RTTI records and before ShellMenu.cpp's globals:
// the scratch pad's bounds, the controllers and the player count.
extern unsigned char g_scratch_pad[0x4000];
void* SPR_BEGIN = g_scratch_pad;
void* SPR_END = g_scratch_pad + 0x4000;
int g_PauseScreenLockedController = -1;
int g_FinalSinglePlayerController = -1;
int g_NumPlayers = 1;

// .sdata, after camera.cpp's RTTI records, before fader.cpp's.
int g_soundStreamRequest = -1;

// .sdata, after pumhandler.cpp's globals, before animated_volume.cpp's RTTI records.
// UserInterface_xfilestime.cpp reads it as a float.
float alpha = 128.0f;
