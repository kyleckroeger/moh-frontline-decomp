/* Adapted from getpic.c of the MPEG Software Simulation Group's mpeg2decode
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

// A fragment of getpic.cpp (0x80100db8): the reference decoder's slice,
// decode_macroblock and Add_Block, with start_of_slice, skipped_macroblock,
// motion_compensation and macroblock_modes inlined. Changes from the
// reference, as the game's code shows: 4:2:0 only (six blocks, chroma
// coordinates halved by the caller), no SNR, spatial or data-partitioning
// layers apart from decode_macroblock's base quantizer update, no Clear_Block
// (Add_Block indexes each row, steps a whole line and clears the block as it
// adds it), GC_IDCT after each coded block, slice taking only MBAmax, and no
// printing. The inlined helpers are declared inline here so that, as in the
// original link, no out-of-line copies remain. The layer-data view is
// inferred; the constants are the reference's (mpeg2dec.h). Decode_Picture
// and Update_Picture_Buffers before slice, which work on the game's
// MPC_CODEC_INTERNAL, are not reconstructed.
#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3
#define D_TYPE 4
#define BOTTOM_FIELD  2
#define FRAME_PICTURE 3
#define MACROBLOCK_INTRA                        1
#define MACROBLOCK_PATTERN                      2
#define MACROBLOCK_MOTION_BACKWARD              4
#define MACROBLOCK_MOTION_FORWARD               8
#define MACROBLOCK_QUANT                        16
#define MB_WEIGHT                  32
#define MB_CLASS4                  64
#define MC_FIELD 1
#define MC_FRAME 2
#define MC_16X8  2
#define MC_DMV   3
#define MV_FIELD 0
#define MV_FRAME 1
#define SC_DP 1
#define SLICE_START_CODE_MIN    0x101
#define SLICE_START_CODE_MAX    0x1AF

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
extern unsigned char *MPDclip;
extern unsigned char *MPDcurrent_frame[3];
extern int MPDcoded_picture_width;
extern int MPDchroma_width;
extern int MPDmb_width;
extern int MPDfault_flag;
// A fragment of getpic.cpp (0x80100950): Decode_Picture, with the reference's
// picture_data and frame-reordering steps inlined. Update_Picture_Buffers
// (rewritten by the game around the codec's frame objects) and slice follow it
// in the original and are not part of this fragment: they are local to the
// file there and declared extern here.
#define FRAME_PICTURE 3
#define B_TYPE 3

class MPC_CODEC_INTERNAL;

extern int MPDpicture_coding_type;
extern int MPDpicture_structure;
extern int MPDmb_width;
extern int MPDmb_height;
extern int MPDsecond_field;
extern int MPDprogressive_frame;

void Update_Picture_Buffers(MPC_CODEC_INTERNAL *codec);
int slice(int MBAmax);

/* decode all macroblocks of the current picture */
/* stages described in ISO/IEC 13818-2 section 7 */
static inline void picture_data()
{
  int MBAmax;
  int ret;

  /* number of macroblocks per picture */
  MBAmax = MPDmb_width*MPDmb_height;

  if (MPDpicture_structure!=FRAME_PICTURE)
    MBAmax>>=1; /* field picture has half as mnay macroblocks as frame */

  for(;;)
  {
    if((ret=slice(MBAmax))<0)
      return;
  }
}

/* track the progressive_frame of the reference frames */
static inline void frame_reorder(int Sequence_Framenum)
{
  /* tracking variables to insure proper output in spatial scalability */
  static int Oldref_progressive_frame, Newref_progressive_frame;

  if (Sequence_Framenum!=0)
  {
    if (MPDpicture_structure==FRAME_PICTURE || MPDsecond_field)
    {
      if (MPDpicture_coding_type!=B_TYPE)
      {
        Newref_progressive_frame = MPDprogressive_frame;
        Oldref_progressive_frame = Newref_progressive_frame;
      }
    }
  }
  else
    Oldref_progressive_frame = MPDprogressive_frame;
}

void Decode_Picture(MPC_CODEC_INTERNAL *codec, int sequence_framenum)
{
  if (MPDpicture_structure==FRAME_PICTURE && MPDsecond_field)
  {
    /* recover from illegal number of field pictures */
    MPDsecond_field = 0;
  }

  /* IMPLEMENTATION: update picture buffer pointers */
  Update_Picture_Buffers(codec);

  /* decode picture data ISO/IEC 13818-2 section 6.2.3.7 */
  picture_data();

  /* ISO/IEC 13818-2 section 6.1.1.11: Frame reordering */
  frame_reorder(sequence_framenum);

  if (MPDpicture_structure!=FRAME_PICTURE)
    MPDsecond_field = !MPDsecond_field;
}
