// unassigned: its zero-initialised globals (0x1d1ba bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct layer_data { unsigned char data[0xa48]; }; // view: size from the original
struct PATHSTATEVIEW { unsigned char data[0x1a8]; }; // view: size from the original
struct PADStatus { unsigned char data[0xc]; }; // view: size from the original
struct MEMCLASS;
struct SNDDRVVIEW { unsigned char data[0xab20]; }; // view: size from the original
struct SNDARAMVIEW { unsigned char data[0x10]; }; // view: size from the original
struct BSClass_struct;
struct CAIObject;
struct AnimSeqAnimInfo_t;
struct CBulletFactory;
struct CWeaponFactory;
typedef void (*UISErrorFnc_t)(); // view: the real signature is in the source that calls it
struct LibraryInfoView;
struct CStaticObject;
struct ISceneNode;
struct CSoundStream;
typedef void (*PADSamplingCallback)(); // view: the real signature is in the source that calls it
struct MIXFXHEADERVIEW;
struct SRCSTATE;
class CAIStatus {
public:
    static CAIObject* s_paioAttacker;
    static CAIObject* s_paioTarget;
};
class CPerformanceCount {
public:
    static int m_ModeCounter0;
    static int m_ModeCounter1;
};

unsigned char g_frame_cache[0xc800];
unsigned char g_scratch_pad[0x4000]; // no source names its type yet
char movementText[14][24];
char MovementText[15][24];
unsigned char * MPDcurrent_frame[3];
int MPDf_code[2][2];
int MPDframe_center_horizontal_offset[3];
int MPDframe_center_vertical_offset[3];
struct layer_data MPDbase;
PATHSTATEVIEW pfstates[4];
PADStatus vgPadStatus[4];
PADStatus gPadStatus[4];
MEMCLASS* memclass[64];
void* sndss[32];
char sndgs[568];
signed char sndaztospkr[256][4];
SNDDRVVIEW snddrv;
SNDARAMVIEW sndaram;
char* g_pLevelBigFile;
char* g_pShellBriefingLogBigFile;
int g_LevelBigFileSize;
int g_ShellBriefingLogBigFileSize;
BSClass_struct* g_pcClassList;
int g_iGermanCreationQueueHeadIndex;
int g_iGermanCreationQueueTailIndex;
int g_iAIObjectCount;
CAIObject* CAIStatus::s_paioAttacker;
CAIObject* CAIStatus::s_paioTarget;
bool f_bObstacleAvoidanceLinesInitialized;
bool g_alarm_activated;
int g_frame_cache_used;
volatile bool g_bDMAToARAM;
volatile bool g_bDMAToMRAM;
unsigned long end;
unsigned int g_amount_allocated;
unsigned int g_num_allocations;
char* g_anim_copy_memory;
unsigned int g_anim_copy_memory_sz;
unsigned int g_anim_copy_memory_cur;
AnimSeqAnimInfo_t* _AnimSeq_CurrentInfo;
CBulletFactory* g_pBulletFactory;
CWeaponFactory* g_pWeaponFactory;
int g_mpTimeLimit;
int g_bMPEndLevel;
UISErrorFnc_t RuntimeErrorFnc;
unsigned char uisEventNotProcess;
void* ScreenInfo;
LibraryInfoView* LibraryInfo;
float g_fScaleValueX;
float g_fScaleValueY;
float g_fScaleValueZ;
float g_fScriptExplosionVelocityScaleOnPlayer;
float g_fScriptExplosionHeightScaleOnPlayer;
CStaticObject* g_pStaticObjectGrenade;
CStaticObject* g_pStaticObjectBazooka;
CStaticObject* g_pStaticObjectStickGrenade;
CStaticObject* g_pStaticObjectKnife;
CStaticObject* g_pStaticObjectWrench;
void* g_pStaticObjectMuzzleFlashNPC;
ISceneNode* g_pStaticObjectMuzzleFlashPlayer;
CSoundStream* pSoundStream;
bool g_bIsRubberGrenadeCheatEnabled;
bool g_bInMultiplayerMode;
unsigned char pathabortmsg[0x4]; // no source names its type yet
unsigned char pathprintf[0x4]; // no source names its type yet
unsigned char pathService[0x4]; // no source names its type yet
unsigned char pathTimer[0x4]; // no source names its type yet
bool SoundHasBeenInitialized;
int gMusicBufSize;
void* gMusicMap;
int CPerformanceCount::m_ModeCounter0;
int CPerformanceCount::m_ModeCounter1;
int MPDquiet_flag;
int MPDtrace_flag;
int MPDfault_flag;
int MPDverbose_flag;
unsigned char * MPDclip;
int MPDcoded_picture_width;
int MPDcoded_picture_height;
int MPDchroma_width;
int MPDchroma_height;
int MPDsecond_field;
int MPDprofile;
int MPDlevel;
int MPDhorizontal_size;
int MPDvertical_size;
int MPDmb_width;
int MPDmb_height;
double MPDbit_rate;
double MPDframe_rate;
int MPDaspect_ratio_information;
int MPDframe_rate_code;
int MPDbit_rate_value;
int MPDvbv_buffer_size;
int MPDconstrained_parameters_flag;
int MPDprofile_and_level_indication;
int MPDprogressive_sequence;
int MPDchroma_format;
int MPDlow_delay;
int MPDframe_rate_extension_n;
int MPDframe_rate_extension_d;
int MPDvideo_format;
int MPDcolor_description;
int MPDcolor_primaries;
int MPDtransfer_characteristics;
int MPDmatrix_coefficients;
int MPDdisplay_horizontal_size;
int MPDdisplay_vertical_size;
int MPDtemporal_reference;
int MPDpicture_coding_type;
int MPDvbv_delay;
int MPDfull_pel_forward_vector;
int MPDforward_f_code;
int MPDfull_pel_backward_vector;
int MPDbackward_f_code;
int MPDintra_dc_precision;
int MPDpicture_structure;
int MPDtop_field_first;
int MPDframe_pred_frame_dct;
int MPDconcealment_motion_vectors;
int MPDintra_vlc_format;
int MPDrepeat_first_field;
int MPDchroma_420_type;
int MPDprogressive_frame;
int MPDcomposite_display_flag;
int MPDv_axis;
int MPDfield_sequence;
int MPDsub_carrier;
int MPDburst_amplitude;
int MPDsub_carrier_phase;
int MPDlayer_id;
int MPDlower_layer_prediction_horizontal_size;
int MPDlower_layer_prediction_vertical_size;
int MPDhorizontal_subsampling_factor_m;
int MPDhorizontal_subsampling_factor_n;
int MPDvertical_subsampling_factor_m;
int MPDvertical_subsampling_factor_n;
int MPDlower_layer_temporal_reference;
int MPDlower_layer_horizontal_offset;
int MPDlower_layer_vertical_offset;
int MPDspatial_temporal_weight_code_table_index;
int MPDlower_layer_progressive_frame;
int MPDlower_layer_deinterlaced_field_select;
int MPDcopyright_flag;
int MPDcopyright_identifier;
int MPDoriginal_or_copy;
int MPDcopyright_number_1;
int MPDcopyright_number_2;
int MPDcopyright_number_3;
int MPDdrop_flag;
int MPDhour;
int MPDminute;
int MPDsec;
int MPDframe;
int MPDclosed_gop;
int MPDbroken_link;
struct layer_data * MPDld;
int MPDtrue_framenum;
PATHSTATEVIEW* pfstate;
unsigned char songprogress[0x4]; // no source names its type yet
signed char inited;
signed char paused;
unsigned int pfactive;
int milliseconds;
unsigned char errno[0x4]; // no source names its type yet
void* FONTstand;
int giPADStarted;
PADSamplingCallback OldCallBack;
int mb_default;
int PRINTinitialized;
volatile int vblticks;
int TIMERhz;
volatile int ticks;
volatile int libticks;
void* MIXinitfn;
void* MIXrestorefn;
void* MIXaudioslicefn;
void* MIXplayinitfn;
void* MIXplayfn;
void* MIXstopfn;
void* MIXsetpitchfn;
MIXFXHEADERVIEW* pheaderinfo;
SRCSTATE** pfilterlist;
void** phead;
void* plisthead;
void* pconhead;
int lastoutputchannels;
int validstructoutputs;
void* modlist;
void* conlist;
int SNDMIXfxv2inited;
