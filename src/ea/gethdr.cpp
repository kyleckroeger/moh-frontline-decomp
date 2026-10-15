/* Adapted from gethdr.c of the MPEG Software Simulation Group's mpeg2decode
   reference decoder (see docs/Licensing.md); its notice follows. */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

// A fragment of gethdr.cpp (0x800ff8fc): the reference decoder's header
// parsers from marker_bit through next_start_code, in the reference's
// definition order (the game emits them reversed). Changes from the reference:
// the decoder's globals carry the game's MPD names, all printing (the verbose
// copyright report included) is absent, and the bit readers are declared here
// returning int, as the signed comparisons of their results show. The
// extension parsers, all inlined, are declared inline here so that, as in the
// original link, no out-of-line copies remain. The layer-data view is
// inferred; the constants and the globals' types are the reference's
// (mpeg2dec.h, global.h). Get_Hdr after these functions, with the
// group-of-pictures and picture headers it inlines, is not reconstructed.
#define ERROR (-1)
#define PICTURE_START_CODE      0x100
#define SLICE_START_CODE_MIN    0x101
#define SLICE_START_CODE_MAX    0x1AF
#define USER_DATA_START_CODE    0x1B2
#define SEQUENCE_HEADER_CODE    0x1B3
#define SEQUENCE_ERROR_CODE     0x1B4
#define EXTENSION_START_CODE    0x1B5
#define SEQUENCE_END_CODE       0x1B7
#define GROUP_START_CODE        0x1B8
#define SYSTEM_START_CODE_MIN   0x1B9
#define SYSTEM_START_CODE_MAX   0x1FF
#define ISO_END_CODE            0x1B9
#define PACK_START_CODE         0x1BA
#define SYSTEM_START_CODE       0x1BB
#define VIDEO_ELEMENTARY_STREAM 0x1e0
#define SC_NONE 0
#define SC_DP   1
#define SC_SPAT 2
#define SC_SNR  3
#define SC_TEMP 4
#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3
#define D_TYPE 4
#define TOP_FIELD     1
#define BOTTOM_FIELD  2
#define FRAME_PICTURE 3
#define MACROBLOCK_INTRA                        1
#define MACROBLOCK_PATTERN                      2
#define MACROBLOCK_MOTION_BACKWARD              4
#define MACROBLOCK_MOTION_FORWARD               8
#define MACROBLOCK_QUANT                        16
#define SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG       32
#define PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS 64
#define MC_FIELD 1
#define MC_FRAME 2
#define MC_16X8  2
#define MC_DMV   3
#define MV_FIELD 0
#define MV_FRAME 1
#define CHROMA420 1
#define CHROMA422 2
#define CHROMA444 3
#define SEQUENCE_EXTENSION_ID                    1
#define SEQUENCE_DISPLAY_EXTENSION_ID            2
#define QUANT_MATRIX_EXTENSION_ID                3
#define COPYRIGHT_EXTENSION_ID                   4
#define SEQUENCE_SCALABLE_EXTENSION_ID           5
#define PICTURE_DISPLAY_EXTENSION_ID             7
#define PICTURE_CODING_EXTENSION_ID              8
#define PICTURE_SPATIAL_SCALABLE_EXTENSION_ID    9
#define PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID  10
#define ZIG_ZAG                                  0
#define PROFILE_422                             (128+5)
#define MAIN_LEVEL                              8
#define NO_LAYER                                0
#define SEQUENCE_LAYER                          1
#define PICTURE_LAYER                           2
#define SLICE_LAYER                             3    
#define MACROBLOCK_LAYER                        4    
#define BLOCK_LAYER                             5
#define EVENT_LAYER                             6
#define ALL_LAYERS                              7
#define FILENAME_LENGTH                       256
#define MB_WEIGHT                  32
#define MB_CLASS4                  64

// The game's layer data: the reference's fields from the bit buffer on, the
// file and read-buffer fields absent (offsets verified by the code).
#define _ANSI_ARGS_(x) x

struct layer_data {
  unsigned char *Rdptr;
  unsigned int Bfr;
  int Incnt;
  int Bitcnt;
  int intra_quantizer_matrix[64];
  int non_intra_quantizer_matrix[64];
  int chroma_intra_quantizer_matrix[64];
  int chroma_non_intra_quantizer_matrix[64];
  int load_intra_quantizer_matrix;
  int load_non_intra_quantizer_matrix;
  int load_chroma_intra_quantizer_matrix;
  int load_chroma_non_intra_quantizer_matrix;
  int MPEG2_Flag;
  int scalable_mode;
  int q_scale_type;
  int alternate_scan;
  int pict_scal;
  int priority_breakpoint;
  int quantizer_scale;
  int intra_slice;
  unsigned char unknown440[8];
  short block[12][64];
};

extern struct layer_data MPDbase;
extern struct layer_data *MPDld;
extern double MPDbit_rate;
extern double MPDframe_rate;
extern int MPDaspect_ratio_information;
extern int MPDbit_rate_value;
extern int MPDburst_amplitude;
extern int MPDchroma_420_type;
extern int MPDchroma_format;
extern int MPDcolor_description;
extern int MPDcolor_primaries;
extern int MPDcomposite_display_flag;
extern int MPDconcealment_motion_vectors;
extern int MPDconstrained_parameters_flag;
extern int MPDcopyright_flag;
extern int MPDcopyright_identifier;
extern int MPDcopyright_number_1;
extern int MPDcopyright_number_2;
extern int MPDcopyright_number_3;
extern int MPDdisplay_horizontal_size;
extern int MPDdisplay_vertical_size;
extern int MPDf_code[2][2];
extern int MPDfield_sequence;
extern int MPDframe_center_horizontal_offset[3];
extern int MPDframe_center_vertical_offset[3];
extern int MPDframe_pred_frame_dct;
extern int MPDframe_rate_code;
extern int MPDframe_rate_extension_d;
extern int MPDframe_rate_extension_n;
extern int MPDhorizontal_size;
extern int MPDhorizontal_subsampling_factor_m;
extern int MPDhorizontal_subsampling_factor_n;
extern int MPDintra_dc_precision;
extern int MPDintra_vlc_format;
extern int MPDlayer_id;
extern int MPDlevel;
extern int MPDlow_delay;
extern int MPDlower_layer_deinterlaced_field_select;
extern int MPDlower_layer_horizontal_offset;
extern int MPDlower_layer_prediction_horizontal_size;
extern int MPDlower_layer_prediction_vertical_size;
extern int MPDlower_layer_progressive_frame;
extern int MPDlower_layer_temporal_reference;
extern int MPDlower_layer_vertical_offset;
extern int MPDmatrix_coefficients;
extern int MPDoriginal_or_copy;
extern int MPDpicture_structure;
extern int MPDprofile;
extern int MPDprofile_and_level_indication;
extern int MPDprogressive_frame;
extern int MPDprogressive_sequence;
extern int MPDrepeat_first_field;
extern int MPDspatial_temporal_weight_code_table_index;
extern int MPDsub_carrier;
extern int MPDsub_carrier_phase;
extern int MPDtop_field_first;
extern int MPDtransfer_characteristics;
extern int MPDv_axis;
extern int MPDvbv_buffer_size;
extern int MPDvertical_size;
extern int MPDvertical_subsampling_factor_m;
extern int MPDvertical_subsampling_factor_n;
extern int MPDvideo_format;
extern unsigned char MPDdefault_intra_quantizer_matrix[64];
extern unsigned char MPDnon_linear_quantizer_scale[32];
extern unsigned char MPDscan[2][64];

int Get_Bits(int N);
int Get_Bits1();
int Show_Bits(int N);
void Flush_Buffer(int N);
int Get_Bits32();
void Flush_Buffer32();
void Error(char *text);
void next_start_code();
void marker_bit(char *text);

/* private prototypes */
static void sequence_header _ANSI_ARGS_((void));
static void extension_and_user_data _ANSI_ARGS_((void));
static inline void sequence_extension _ANSI_ARGS_((void));
static inline void sequence_display_extension _ANSI_ARGS_((void));
static inline void quant_matrix_extension _ANSI_ARGS_((void));
static inline void sequence_scalable_extension _ANSI_ARGS_((void));
static inline void picture_display_extension _ANSI_ARGS_((void));
static inline void picture_coding_extension _ANSI_ARGS_((void));
static inline void picture_spatial_scalable_extension _ANSI_ARGS_((void));
static inline void picture_temporal_scalable_extension _ANSI_ARGS_((void));
static inline int  extra_bit_information _ANSI_ARGS_((void));
static inline void copyright_extension _ANSI_ARGS_((void));
static inline void user_data _ANSI_ARGS_((void));
static inline void user_data _ANSI_ARGS_((void));




/* private variables */

#define RESERVED    -1 
static double frame_rate_Table[16] =
{
  0.0,
  ((23.0*1000.0)/1001.0),
  24.0,
  25.0,
  ((30.0*1000.0)/1001.0),
  30.0,
  50.0,
  ((60.0*1000.0)/1001.0),
  60.0,
 
  RESERVED,
  RESERVED,
  RESERVED,
  RESERVED,
  RESERVED,
  RESERVED,
  RESERVED
};


/* align to start of next next_start_code */

void next_start_code()
{
  /* byte align */
  Flush_Buffer(MPDld->Incnt&7);
  while (Show_Bits(24)!=0x01L)
    Flush_Buffer(8);
}


/* decode sequence header */

static void sequence_header()
{
  int i;
  int pos;

  pos = MPDld->Bitcnt;
  MPDhorizontal_size             = Get_Bits(12);
  MPDvertical_size               = Get_Bits(12);
  MPDaspect_ratio_information    = Get_Bits(4);
  MPDframe_rate_code             = Get_Bits(4);
  MPDbit_rate_value              = Get_Bits(18);
  marker_bit("sequence_header()");
  MPDvbv_buffer_size             = Get_Bits(10);
  MPDconstrained_parameters_flag = Get_Bits(1);

  if((MPDld->load_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
      MPDld->intra_quantizer_matrix[MPDscan[ZIG_ZAG][i]] = Get_Bits(8);
  }
  else
  {
    for (i=0; i<64; i++)
      MPDld->intra_quantizer_matrix[i] = MPDdefault_intra_quantizer_matrix[i];
  }

  if((MPDld->load_non_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
      MPDld->non_intra_quantizer_matrix[MPDscan[ZIG_ZAG][i]] = Get_Bits(8);
  }
  else
  {
    for (i=0; i<64; i++)
      MPDld->non_intra_quantizer_matrix[i] = 16;
  }

  /* copy luminance to chrominance matrices */
  for (i=0; i<64; i++)
  {
    MPDld->chroma_intra_quantizer_matrix[i] =
      MPDld->intra_quantizer_matrix[i];

    MPDld->chroma_non_intra_quantizer_matrix[i] =
      MPDld->non_intra_quantizer_matrix[i];
  }



  extension_and_user_data();
}




/* decode picture header */

/* decode slice header */

/* ISO/IEC 13818-2 section 6.2.4 */
int slice_header()
{
  int slice_vertical_position_extension;
  int quantizer_scale_code;
  int pos;
  int slice_picture_id_enable = 0;
  int slice_picture_id = 0;
  int extra_information_slice = 0;

  pos = MPDld->Bitcnt;

  slice_vertical_position_extension =
    (MPDld->MPEG2_Flag && MPDvertical_size>2800) ? Get_Bits(3) : 0;

  if (MPDld->scalable_mode==SC_DP)
    MPDld->priority_breakpoint = Get_Bits(7);

  quantizer_scale_code = Get_Bits(5);
  MPDld->quantizer_scale =
    MPDld->MPEG2_Flag ? (MPDld->q_scale_type ? MPDnon_linear_quantizer_scale[quantizer_scale_code] : quantizer_scale_code<<1) : quantizer_scale_code;

  /* slice_id introduced in March 1995 as part of the video corridendum
     (after the IS was drafted in November 1994) */
  if (Get_Bits(1))
  {
    MPDld->intra_slice = Get_Bits(1);

    slice_picture_id_enable = Get_Bits(1);
	slice_picture_id = Get_Bits(6);

    extra_information_slice = extra_bit_information();
  }
  else
    MPDld->intra_slice = 0;




  return slice_vertical_position_extension;
}


/* decode extension and user data */
/* ISO/IEC 13818-2 section 6.2.2.2 */
static void extension_and_user_data()
{
  int code,ext_ID;

  next_start_code();

  while ((code = Show_Bits(32))==EXTENSION_START_CODE || code==USER_DATA_START_CODE)
  {
    if (code==EXTENSION_START_CODE)
    {
      Flush_Buffer32();
      ext_ID = Get_Bits(4);
      switch (ext_ID)
      {
      case SEQUENCE_EXTENSION_ID:
        sequence_extension();
        break;
      case SEQUENCE_DISPLAY_EXTENSION_ID:
        sequence_display_extension();
        break;
      case QUANT_MATRIX_EXTENSION_ID:
        quant_matrix_extension();
        break;
      case SEQUENCE_SCALABLE_EXTENSION_ID:
        sequence_scalable_extension();
        break;
      case PICTURE_DISPLAY_EXTENSION_ID:
        picture_display_extension();
        break;
      case PICTURE_CODING_EXTENSION_ID:
        picture_coding_extension();
        break;
      case PICTURE_SPATIAL_SCALABLE_EXTENSION_ID:
        picture_spatial_scalable_extension();
        break;
      case PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID:
        picture_temporal_scalable_extension();
        break;
      case COPYRIGHT_EXTENSION_ID:
        copyright_extension();
        break;
     default:
        break;
      }
      next_start_code();
    }
    else
    {
      Flush_Buffer32();
      user_data();
    }
  }
}


/* decode sequence extension */

/* ISO/IEC 13818-2 section 6.2.2.3 */
static inline void sequence_extension()
{
  int horizontal_size_extension;
  int vertical_size_extension;
  int bit_rate_extension;
  int vbv_buffer_size_extension;


  MPDld->MPEG2_Flag = 1;

  MPDld->scalable_mode = SC_NONE; /* unless overwritten by sequence_scalable_extension() */
  MPDlayer_id = 0;                /* unless overwritten by sequence_scalable_extension() */
  
  MPDprofile_and_level_indication = Get_Bits(8);
  MPDprogressive_sequence         = Get_Bits(1);
  MPDchroma_format                = Get_Bits(2);
  horizontal_size_extension    = Get_Bits(2);
  vertical_size_extension      = Get_Bits(2);
  bit_rate_extension           = Get_Bits(12);
  marker_bit("sequence_extension");
  vbv_buffer_size_extension    = Get_Bits(8);
  MPDlow_delay                    = Get_Bits(1);
  MPDframe_rate_extension_n       = Get_Bits(2);
  MPDframe_rate_extension_d       = Get_Bits(5);

  MPDframe_rate = frame_rate_Table[MPDframe_rate_code] *
    ((MPDframe_rate_extension_n+1)/(MPDframe_rate_extension_d+1));

  /* special case for 422 profile & level must be made */
  if((MPDprofile_and_level_indication>>7) & 1)
  {  /* escape bit of profile_and_level_indication set */
  
    /* 4:2:2 Profile @ Main Level */
    if((MPDprofile_and_level_indication&15)==5)
    {
      MPDprofile = PROFILE_422;
      MPDlevel   = MAIN_LEVEL;  
    }
  }
  else
  {
    MPDprofile = MPDprofile_and_level_indication >> 4;  /* Profile is upper nibble */
    MPDlevel   = MPDprofile_and_level_indication & 0xF;  /* Level is lower nibble */
  }
  
 
  MPDhorizontal_size = (horizontal_size_extension<<12) | (MPDhorizontal_size&0x0fff);
  MPDvertical_size = (vertical_size_extension<<12) | (MPDvertical_size&0x0fff);


  /* ISO/IEC 13818-2 does not define bit_rate_value to be composed of
   * both the original bit_rate_value parsed in sequence_header() and
   * the optional bit_rate_extension in sequence_extension_header(). 
   * However, we use it for bitstream verification purposes. 
   */

  MPDbit_rate_value += (bit_rate_extension << 18);
  MPDbit_rate = ((double) MPDbit_rate_value) * 400.0;
  MPDvbv_buffer_size += (vbv_buffer_size_extension << 10);




}


/* decode sequence display extension */

static inline void sequence_display_extension()
{
  int pos;

  pos = MPDld->Bitcnt;
  MPDvideo_format      = Get_Bits(3);
  MPDcolor_description = Get_Bits(1);

  if (MPDcolor_description)
  {
    MPDcolor_primaries          = Get_Bits(8);
    MPDtransfer_characteristics = Get_Bits(8);
    MPDmatrix_coefficients      = Get_Bits(8);
  }

  MPDdisplay_horizontal_size = Get_Bits(14);
  marker_bit("sequence_display_extension");
  MPDdisplay_vertical_size   = Get_Bits(14);



}


/* decode quant matrix entension */
/* ISO/IEC 13818-2 section 6.2.3.2 */
static inline void quant_matrix_extension()
{
  int i;
  int pos;

  pos = MPDld->Bitcnt;

  if((MPDld->load_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
    {
      MPDld->chroma_intra_quantizer_matrix[MPDscan[ZIG_ZAG][i]]
      = MPDld->intra_quantizer_matrix[MPDscan[ZIG_ZAG][i]]
      = Get_Bits(8);
    }
  }

  if((MPDld->load_non_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
    {
      MPDld->chroma_non_intra_quantizer_matrix[MPDscan[ZIG_ZAG][i]]
      = MPDld->non_intra_quantizer_matrix[MPDscan[ZIG_ZAG][i]]
      = Get_Bits(8);
    }
  }

  if((MPDld->load_chroma_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
      MPDld->chroma_intra_quantizer_matrix[MPDscan[ZIG_ZAG][i]] = Get_Bits(8);
  }

  if((MPDld->load_chroma_non_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
      MPDld->chroma_non_intra_quantizer_matrix[MPDscan[ZIG_ZAG][i]] = Get_Bits(8);
  }



}


/* decode sequence scalable extension */
/* ISO/IEC 13818-2   section 6.2.2.5 */
static inline void sequence_scalable_extension()
{
  int pos;

  pos = MPDld->Bitcnt;

  /* values (without the +1 offset) of scalable_mode are defined in 
     Table 6-10 of ISO/IEC 13818-2 */
  MPDld->scalable_mode = Get_Bits(2) + 1; /* add 1 to make SC_DP != SC_NONE */

  MPDlayer_id = Get_Bits(4);

  if (MPDld->scalable_mode==SC_SPAT)
  {
    MPDlower_layer_prediction_horizontal_size = Get_Bits(14);
    marker_bit("sequence_scalable_extension()");
    MPDlower_layer_prediction_vertical_size   = Get_Bits(14); 
    MPDhorizontal_subsampling_factor_m        = Get_Bits(5);
    MPDhorizontal_subsampling_factor_n        = Get_Bits(5);
    MPDvertical_subsampling_factor_m          = Get_Bits(5);
    MPDvertical_subsampling_factor_n          = Get_Bits(5);
  }

  if (MPDld->scalable_mode==SC_TEMP)
    Error("temporal scalability not implemented\n");



}


/* decode picture display extension */
/* ISO/IEC 13818-2 section 6.2.3.3. */
static inline void picture_display_extension()
{
  int i;
  int number_of_frame_center_offsets;
  int pos;

  pos = MPDld->Bitcnt;
  /* based on ISO/IEC 13818-2 section 6.3.12 
    (November 1994) Picture display extensions */

  /* derive number_of_frame_center_offsets */
  if(MPDprogressive_sequence)
  {
    if(MPDrepeat_first_field)
    {
      if(MPDtop_field_first)
        number_of_frame_center_offsets = 3;
      else
        number_of_frame_center_offsets = 2;
    }
    else
    {
      number_of_frame_center_offsets = 1;
    }
  }
  else
  {
    if(MPDpicture_structure!=FRAME_PICTURE)
    {
      number_of_frame_center_offsets = 1;
    }
    else
    {
      if(MPDrepeat_first_field)
        number_of_frame_center_offsets = 3;
      else
        number_of_frame_center_offsets = 2;
    }
  }


  /* now parse */
  for (i=0; i<number_of_frame_center_offsets; i++)
  {
    MPDframe_center_horizontal_offset[i] = Get_Bits(16);
    marker_bit("picture_display_extension, first marker bit");
    
    MPDframe_center_vertical_offset[i]   = Get_Bits(16);
    marker_bit("picture_display_extension, second marker bit");
  }



}


/* decode picture coding extension */
static inline void picture_coding_extension()
{
  int pos;

  pos = MPDld->Bitcnt;

  MPDf_code[0][0] = Get_Bits(4);
  MPDf_code[0][1] = Get_Bits(4);
  MPDf_code[1][0] = Get_Bits(4);
  MPDf_code[1][1] = Get_Bits(4);

  MPDintra_dc_precision         = Get_Bits(2);
  MPDpicture_structure          = Get_Bits(2);
  MPDtop_field_first            = Get_Bits(1);
  MPDframe_pred_frame_dct       = Get_Bits(1);
  MPDconcealment_motion_vectors = Get_Bits(1);
  MPDld->q_scale_type           = Get_Bits(1);
  MPDintra_vlc_format           = Get_Bits(1);
  MPDld->alternate_scan         = Get_Bits(1);
  MPDrepeat_first_field         = Get_Bits(1);
  MPDchroma_420_type            = Get_Bits(1);
  MPDprogressive_frame          = Get_Bits(1);
  MPDcomposite_display_flag     = Get_Bits(1);

  if (MPDcomposite_display_flag)
  {
    MPDv_axis            = Get_Bits(1);
    MPDfield_sequence    = Get_Bits(3);
    MPDsub_carrier       = Get_Bits(1);
    MPDburst_amplitude   = Get_Bits(7);
    MPDsub_carrier_phase = Get_Bits(8);
  }


}


/* decode picture spatial scalable extension */
/* ISO/IEC 13818-2 section 6.2.3.5. */
static inline void picture_spatial_scalable_extension()
{
  int pos;

  pos = MPDld->Bitcnt;

  MPDld->pict_scal = 1; /* use spatial scalability in this picture */

  MPDlower_layer_temporal_reference = Get_Bits(10);
  marker_bit("picture_spatial_scalable_extension(), first marker bit");
  MPDlower_layer_horizontal_offset = Get_Bits(15);
  if (MPDlower_layer_horizontal_offset>=16384)
    MPDlower_layer_horizontal_offset-= 32768;
  marker_bit("picture_spatial_scalable_extension(), second marker bit");
  MPDlower_layer_vertical_offset = Get_Bits(15);
  if (MPDlower_layer_vertical_offset>=16384)
    MPDlower_layer_vertical_offset-= 32768;
  MPDspatial_temporal_weight_code_table_index = Get_Bits(2);
  MPDlower_layer_progressive_frame = Get_Bits(1);
  MPDlower_layer_deinterlaced_field_select = Get_Bits(1);



}


/* decode picture temporal scalable extension
 *
 * not implemented
 */
/* ISO/IEC 13818-2 section 6.2.3.4. */
static inline void picture_temporal_scalable_extension()
{
  Error("temporal scalability not supported\n");

}


/* decode extra bit information */
/* ISO/IEC 13818-2 section 6.2.3.4. */
static inline int extra_bit_information()
{
  int Byte_Count = 0;

  while (Get_Bits1())
  {
    Flush_Buffer(8);
    Byte_Count++;
  }

  return(Byte_Count);
}



/* ISO/IEC 13818-2 section 5.3 */
/* Purpose: this function is mainly designed to aid in bitstream conformance
   testing.  A simple Flush_Buffer(1) would do */
void marker_bit(char *text)
{
  int marker;

  (void) text;

  marker = Get_Bits(1);

}



/* ISO/IEC 13818-2  sections 6.3.4.1 and 6.2.2.2.2 */
static inline void user_data()
{
  /* skip ahead to the next start code */
  next_start_code();
}



/* Copyright extension */
/* ISO/IEC 13818-2 section 6.2.3.6. */
/* (header added in November, 1994 to the IS document) */


static inline void copyright_extension()
{
  int pos;
  int reserved_data;

  pos = MPDld->Bitcnt;
  

  MPDcopyright_flag =       Get_Bits(1); 
  MPDcopyright_identifier = Get_Bits(8);
  MPDoriginal_or_copy =     Get_Bits(1);
  
  /* reserved */
  reserved_data = Get_Bits(7);

  marker_bit("copyright_extension(), first marker bit");
  MPDcopyright_number_1 =   Get_Bits(20);
  marker_bit("copyright_extension(), second marker bit");
  MPDcopyright_number_2 =   Get_Bits(22);
  marker_bit("copyright_extension(), third marker bit");
  MPDcopyright_number_3 =   Get_Bits(22);


}


