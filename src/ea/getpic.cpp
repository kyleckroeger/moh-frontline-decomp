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

// getpic.cpp from Decode_Picture (0x80100950) to the end of its code: the
// reference decoder's Decode_Picture (with picture_data and the
// frame-reordering step inlined), Update_Picture_Buffers, slice,
// decode_macroblock and Add_Block, with start_of_slice, skipped_macroblock,
// motion_compensation and macroblock_modes inlined. Changes from the
// reference, as the game's code shows: Update_Picture_Buffers is rewritten
// around the game's codec frame objects (each frame's image holds the three
// planes back to back); 4:2:0 only (six blocks, chroma coordinates halved by
// the caller), no SNR, spatial or data-partitioning layers apart from
// decode_macroblock's base quantizer update, no Clear_Block (Add_Block indexes
// each row, steps a whole line and clears the block as it adds it), GC_IDCT
// after each coded block, slice taking only MBAmax, and no printing. The
// inlined helpers are declared inline here so that, as in the original link,
// no out-of-line copies remain. With deferred inlining the functions are
// emitted in reverse source order. The layer-data, codec, frame and image
// views are inferred; the constants are the reference's (mpeg2dec.h).
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
extern int MPDpicture_coding_type;
extern int MPDpicture_structure;
extern int MPDframe_pred_frame_dct;
extern int MPDconcealment_motion_vectors;
extern int MPDspatial_temporal_weight_code_table_index;
extern int MPDf_code[2][2];
extern int MPDfull_pel_forward_vector;
extern int MPDforward_f_code;
extern int MPDfull_pel_backward_vector;
extern int MPDbackward_f_code;
extern unsigned char MPDnon_linear_quantizer_scale[32];

int Get_Bits(int N);
int Show_Bits(int N);
void Flush_Buffer(int N);
void Flush_Buffer32();
void next_start_code();
int slice_header();
void marker_bit(char *text);
int Get_macroblock_type();
int Get_macroblock_address_increment();
int Get_coded_block_pattern();
void motion_vectors(int PMV[2][2][2], int dmvector[2], int motion_vertical_field_select[2][2],
  int s, int motion_vector_count, int mv_format, int h_r_size, int v_r_size, int dmv, int mvscale);
void motion_vector(int *PMV, int *dmvector, int h_r_size, int v_r_size, int dmv, int mvscale,
  int full_pel_vector);
void form_predictions(int bx, int by, int macroblock_type, int motion_type, int PMV[2][2][2],
  int motion_vertical_field_select[2][2], int dmvector[2], int stwtype);
void Decode_MPEG1_Intra_Block(int comp, int dc_dct_pred[]);
void Decode_MPEG1_Non_Intra_Block(int comp);
void Decode_MPEG2_Intra_Block(int comp, int dc_dct_pred[]);
void Decode_MPEG2_Non_Intra_Block(int comp);
extern "C" void GC_IDCT(short *block);

// Inferred views of the game's movie classes, limited to what this file uses.
// A frame's image keeps its pixels after a 16-byte header or, when a flag bit
// in its byte at +12 is set, at a stored offset.
struct MPDImageView {
  unsigned char unknown00[4];
  short width;
  short height;
  unsigned char unknown08[4];
  unsigned char unknown0cBits : 3;
  unsigned char hasDataOffset : 1;
  unsigned char unknown0cLow : 4;
  unsigned char unknown0d[3];
  int dataOffset;

  unsigned char *Data()
  {
    return hasDataOffset ? (unsigned char *)this + dataOffset : (unsigned char *)this + 16;
  }
};

namespace RCMP {
struct FRAME {
  void *unknown00;
  MPDImageView *image;
};

class CODEC {
public:
  virtual ~CODEC();
};
}

class MPC_CODEC_INTERNAL : public RCMP::CODEC {
public:
  virtual ~MPC_CODEC_INTERNAL();
  virtual void Init();
  virtual void GetFrame();
  virtual void GetCurrentFrameNumber();
  virtual void GetFrameRate();
  virtual void ReleaseFrame(RCMP::FRAME *frame);

  RCMP::FRAME *CreateB();
  RCMP::FRAME *CreateIorP();

  RCMP::FRAME *forward;
  RCMP::FRAME *backward;
  RCMP::FRAME *current;
  RCMP::FRAME *output;
};

extern unsigned char *MPDbackward_reference_frame[3];
extern unsigned char *MPDforward_reference_frame[3];
extern int MPDmb_height;
extern int MPDsecond_field;
extern int MPDprogressive_frame;

static void Add_Block(int comp, int bx, int by, int dct_type, int addflag)
{
  int cc,i, j, iincr;
  unsigned char *rfp;
  short *bp;

  bp = MPDld->block[comp];

  /* derive color component index */
  /* equivalent to ISO/IEC 13818-2 Table 7-1 */
  cc = (comp<4) ? 0 : (comp&1)+1; /* color component index */

  if (cc==0)
  {
    /* luminance */

    if (MPDpicture_structure==FRAME_PICTURE)
      if (dct_type)
      {
        /* field DCT coding */
        rfp = MPDcurrent_frame[0]
              + MPDcoded_picture_width*(by+((comp&2)>>1)) + bx + ((comp&1)<<3);
        iincr = (MPDcoded_picture_width<<1);
      }
      else
      {
        /* frame DCT coding */
        rfp = MPDcurrent_frame[0]
              + MPDcoded_picture_width*(by+((comp&2)<<2)) + bx + ((comp&1)<<3);
        iincr = MPDcoded_picture_width;
      }
    else
    {
      /* field picture */
      rfp = MPDcurrent_frame[0]
            + (MPDcoded_picture_width<<1)*(by+((comp&2)<<2)) + bx + ((comp&1)<<3);
      iincr = (MPDcoded_picture_width<<1);
    }
  }
  else
  {
    /* chrominance */

    if (MPDpicture_structure==FRAME_PICTURE)
    {
      /* frame DCT coding */
      rfp = MPDcurrent_frame[cc]
            + MPDchroma_width*(by+((comp&2)<<2)) + bx + (comp&8);
      iincr = MPDchroma_width;
    }
    else
    {
      /* field picture */
      rfp = MPDcurrent_frame[cc]
            + (MPDchroma_width<<1)*(by+((comp&2)<<2)) + bx + (comp&8);
      iincr = (MPDchroma_width<<1);
    }
  }

  if (addflag)
  {
    for (i=0; i<8; i++)
    {
      for (j=0; j<8; j++)
      {
        rfp[j] = MPDclip[bp[j] + rfp[j]];
        bp[j] = 0;
      }
      rfp+= iincr;
      bp+= 8;
    }
  }
  else
  {
    for (i=0; i<8; i++)
    {
      for (j=0; j<8; j++)
      {
        rfp[j] = MPDclip[bp[j] + 128];
        bp[j] = 0;
      }
      rfp+= iincr;
      bp+= 8;
    }
  }
}

/* ISO/IEC 13818-2 section 6.3.17.1: Macroblock modes */
static inline void macroblock_modes(int *pmacroblock_type, int *pstwtype, int *pstwclass,
  int *pmotion_type, int *pmotion_vector_count, int *pmv_format, int *pdmv, int *pmvscale,
  int *pdct_type)
{
  int macroblock_type;
  int stwtype, stwcode, stwclass;
  int motion_type = 0;
  int motion_vector_count, mv_format, dmv, mvscale;
  int dct_type;
  static unsigned char stwc_table[3][4]
    = { {6,3,7,4}, {2,1,5,4}, {2,5,7,4} };
  static unsigned char stwclass_table[9]
    = {0, 1, 2, 1, 1, 2, 3, 3, 4};

  /* get macroblock_type */
  macroblock_type = Get_macroblock_type();

  if (MPDfault_flag) return;

  /* get spatial_temporal_weight_code */
  if (macroblock_type & MB_WEIGHT)
  {
    if (MPDspatial_temporal_weight_code_table_index==0)
      stwtype = 4;
    else
    {
      stwcode = Get_Bits(2);
      stwtype = stwc_table[MPDspatial_temporal_weight_code_table_index-1][stwcode];
    }
  }
  else
    stwtype = (macroblock_type & MB_CLASS4) ? 8 : 0;

  /* SCALABILITY: derive spatial_temporal_weight_class (Table 7-18) */
  stwclass = stwclass_table[stwtype];

  /* get frame/field motion type */
  if (macroblock_type & (MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD))
  {
    if (MPDpicture_structure==FRAME_PICTURE) /* frame_motion_type */
    {
      motion_type = MPDframe_pred_frame_dct ? MC_FRAME : Get_Bits(2);
    }
    else /* field_motion_type */
    {
      motion_type = Get_Bits(2);
    }
  }
  else if ((macroblock_type & MACROBLOCK_INTRA) && MPDconcealment_motion_vectors)
  {
    /* concealment motion vectors */
    motion_type = (MPDpicture_structure==FRAME_PICTURE) ? MC_FRAME : MC_FIELD;
  }

  /* derive motion_vector_count, mv_format and dmv, (table 6-17, 6-18) */
  if (MPDpicture_structure==FRAME_PICTURE)
  {
    motion_vector_count = (motion_type==MC_FIELD && stwclass<2) ? 2 : 1;
    mv_format = (motion_type==MC_FRAME) ? MV_FRAME : MV_FIELD;
  }
  else
  {
    motion_vector_count = (motion_type==MC_16X8) ? 2 : 1;
    mv_format = MV_FIELD;
  }

  dmv = (motion_type==MC_DMV); /* dual prime */

  /* field mv predictions in frame pictures have to be scaled */
  mvscale = ((mv_format==MV_FIELD) && (MPDpicture_structure==FRAME_PICTURE));

  /* get dct_type (frame DCT / field DCT) */
  dct_type = (MPDpicture_structure==FRAME_PICTURE)
             && (!MPDframe_pred_frame_dct)
             && (macroblock_type & (MACROBLOCK_PATTERN|MACROBLOCK_INTRA))
             ? Get_Bits(1)
             : 0;

  /* return values */
  *pmacroblock_type = macroblock_type;
  *pstwtype = stwtype;
  *pstwclass = stwclass;
  *pmotion_type = motion_type;
  *pmotion_vector_count = motion_vector_count;
  *pmv_format = mv_format;
  *pdmv = dmv;
  *pmvscale = mvscale;
  *pdct_type = dct_type;
}

static int decode_macroblock(int *macroblock_type, int *stwtype, int *stwclass, int *motion_type,
  int *dct_type, int PMV[2][2][2], int dc_dct_pred[3], int motion_vertical_field_select[2][2],
  int dmvector[2])
{
  /* locals */
  int quantizer_scale_code;
  int comp;

  int motion_vector_count;
  int mv_format;
  int dmv;
  int mvscale;
  int coded_block_pattern;

  /* ISO/IEC 13818-2 section 6.3.17.1: Macroblock modes */
  macroblock_modes(macroblock_type, stwtype, stwclass,
    motion_type, &motion_vector_count, &mv_format, &dmv, &mvscale,
    dct_type);

  if (MPDfault_flag) return(0);  /* trigger: go to next slice */

  if (*macroblock_type & MACROBLOCK_QUANT)
  {
    quantizer_scale_code = Get_Bits(5);

    /* ISO/IEC 13818-2 section 7.4.2.2: Quantizer scale factor */
    if (MPDld->MPEG2_Flag)
      MPDld->quantizer_scale =
      MPDld->q_scale_type ? MPDnon_linear_quantizer_scale[quantizer_scale_code]
       : (quantizer_scale_code << 1);
    else
      MPDld->quantizer_scale = quantizer_scale_code;

    /* SCALABILITY: Data Partitioning */
    if (MPDbase.scalable_mode==SC_DP)
      /* make sure base.quantizer_scale is valid */
      MPDbase.quantizer_scale = MPDld->quantizer_scale;
  }

  /* motion vectors */

  /* ISO/IEC 13818-2 section 6.3.17.2: Motion vectors */

  /* decode forward motion vectors */
  if ((*macroblock_type & MACROBLOCK_MOTION_FORWARD)
    || ((*macroblock_type & MACROBLOCK_INTRA)
    && MPDconcealment_motion_vectors))
  {
    if (MPDld->MPEG2_Flag)
      motion_vectors(PMV,dmvector,motion_vertical_field_select,
        0,motion_vector_count,mv_format,MPDf_code[0][0]-1,MPDf_code[0][1]-1,
        dmv,mvscale);
    else
      motion_vector(PMV[0][0],dmvector,
      MPDforward_f_code-1,MPDforward_f_code-1,0,0,MPDfull_pel_forward_vector);
  }

  if (MPDfault_flag) return(0);  /* trigger: go to next slice */

  /* decode backward motion vectors */
  if (*macroblock_type & MACROBLOCK_MOTION_BACKWARD)
  {
    if (MPDld->MPEG2_Flag)
      motion_vectors(PMV,dmvector,motion_vertical_field_select,
        1,motion_vector_count,mv_format,MPDf_code[1][0]-1,MPDf_code[1][1]-1,0,
        mvscale);
    else
      motion_vector(PMV[0][1],dmvector,
        MPDbackward_f_code-1,MPDbackward_f_code-1,0,0,MPDfull_pel_backward_vector);
  }

  if (MPDfault_flag) return(0);  /* trigger: go to next slice */

  if ((*macroblock_type & MACROBLOCK_INTRA) && MPDconcealment_motion_vectors)
    Flush_Buffer(1); /* remove marker_bit */

  /* macroblock_pattern */
  /* ISO/IEC 13818-2 section 6.3.17.4: Coded block pattern */
  if (*macroblock_type & MACROBLOCK_PATTERN)
    coded_block_pattern = Get_coded_block_pattern();
  else
    coded_block_pattern = (*macroblock_type & MACROBLOCK_INTRA) ?
      (1<<6)-1 : 0;

  if (MPDfault_flag) return(0);  /* trigger: go to next slice */

  /* decode blocks */
  for (comp=0; comp<6; comp++)
  {
    if (coded_block_pattern & (1<<(6-1-comp)))
    {
      if (*macroblock_type & MACROBLOCK_INTRA)
      {
        if (MPDld->MPEG2_Flag)
          Decode_MPEG2_Intra_Block(comp,dc_dct_pred);
        else
          Decode_MPEG1_Intra_Block(comp,dc_dct_pred);
      }
      else
      {
        if (MPDld->MPEG2_Flag)
          Decode_MPEG2_Non_Intra_Block(comp);
        else
          Decode_MPEG1_Non_Intra_Block(comp);
      }

      GC_IDCT(MPDld->block[comp]);

      if (MPDfault_flag) return(0);  /* trigger: go to next slice */
    }
  }

  if(MPDpicture_coding_type==D_TYPE)
  {
    /* remove end_of_macroblock (always 1, prevents startcode emulation) */
    /* ISO/IEC 11172-2 section 2.4.2.7 and 2.4.3.6 */
    marker_bit("D picture end_of_macroblock bit");
  }

  /* reset intra_dc predictors */
  /* ISO/IEC 13818-2 section 7.2.1: DC coefficients in intra blocks */
  if (!(*macroblock_type & MACROBLOCK_INTRA))
    dc_dct_pred[0]=dc_dct_pred[1]=dc_dct_pred[2]=0;

  /* reset motion vector predictors */
  if ((*macroblock_type & MACROBLOCK_INTRA) && !MPDconcealment_motion_vectors)
  {
    /* intra mb without concealment motion vectors */
    /* ISO/IEC 13818-2 section 7.6.3.4: Resetting motion vector predictors */
    PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;
    PMV[0][1][0]=PMV[0][1][1]=PMV[1][1][0]=PMV[1][1][1]=0;
  }

  /* special "No_MC" macroblock_type case */
  /* ISO/IEC 13818-2 section 7.6.3.5: Prediction in P pictures */
  if ((MPDpicture_coding_type==P_TYPE)
    && !(*macroblock_type & (MACROBLOCK_MOTION_FORWARD|MACROBLOCK_INTRA)))
  {
    /* non-intra mb without forward mv in a P picture */
    /* ISO/IEC 13818-2 section 7.6.3.4: Resetting motion vector predictors */
    PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;

    /* derive motion_type */
    /* ISO/IEC 13818-2 section 6.3.17.1: Macroblock modes, frame_motion_type */
    if (MPDpicture_structure==FRAME_PICTURE)
      *motion_type = MC_FRAME;
    else
    {
      *motion_type = MC_FIELD;
      /* predict from field of same parity */
      motion_vertical_field_select[0][0] = (MPDpicture_structure==BOTTOM_FIELD);
    }
  }

  if (*stwclass==4)
  {
    /* purely spatially predicted macroblock */
    /* ISO/IEC 13818-2 section 7.7.5.1: Resetting motion vector predictions */
    PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;
    PMV[0][1][0]=PMV[0][1][1]=PMV[1][1][0]=PMV[1][1][1]=0;
  }

  /* successfully decoded macroblock */
  return(1);

} /* decode_macroblock */

/* return==-1 means go to next picture */
/* the expression "start of slice" is used throughout the normative
   body of the MPEG specification */
static inline int start_of_slice(int MBAmax, int *MBA, int *MBAinc,
  int dc_dct_pred[3], int PMV[2][2][2])
{
  unsigned int code;
  int slice_vert_pos_ext;

  MPDld = &MPDbase;

  MPDfault_flag = 0;

  next_start_code();
  code = Show_Bits(32);

  if (code<SLICE_START_CODE_MIN || code>SLICE_START_CODE_MAX)
  {
    /* only slice headers are allowed in picture_data */
    return(-1);  /* trigger: end of picture */
  }

  Flush_Buffer32();

  /* decode slice header (may change quantizer_scale) */
  slice_vert_pos_ext = slice_header();

  /* decode macroblock address increment */
  *MBAinc = Get_macroblock_address_increment();

  if (MPDfault_flag)
  {
    return(0);   /* trigger: go to next slice */
  }

  /* set current location */
  *MBA = ((slice_vert_pos_ext<<7) + (code&255) - 1)*MPDmb_width + *MBAinc - 1;
  *MBAinc = 1; /* first macroblock in slice: not skipped */

  /* reset all DC coefficient and motion vector predictors */
  /* ISO/IEC 13818-2 section 7.2.1: DC coefficients in intra blocks */
  dc_dct_pred[0]=dc_dct_pred[1]=dc_dct_pred[2]=0;

  /* ISO/IEC 13818-2 section 7.6.3.4: Resetting motion vector predictors */
  PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;
  PMV[0][1][0]=PMV[0][1][1]=PMV[1][1][0]=PMV[1][1][1]=0;

  /* successfull: trigger decode macroblocks in slice */
  return(1);
}

/* ISO/IEC 13818-2 section 7.6.6 */
static inline void skipped_macroblock(int dc_dct_pred[3], int PMV[2][2][2], int *motion_type,
  int motion_vertical_field_select[2][2], int *stwtype, int *macroblock_type)
{
  /* reset intra_dc predictors */
  /* ISO/IEC 13818-2 section 7.2.1: DC coefficients in intra blocks */
  dc_dct_pred[0]=dc_dct_pred[1]=dc_dct_pred[2]=0;

  /* reset motion vector predictors */
  /* ISO/IEC 13818-2 section 7.6.3.4: Resetting motion vector predictors */
  if (MPDpicture_coding_type==P_TYPE)
    PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;

  /* derive motion_type */
  if (MPDpicture_structure==FRAME_PICTURE)
    *motion_type = MC_FRAME;
  else
  {
    *motion_type = MC_FIELD;

    /* predict from field of same parity */
    motion_vertical_field_select[0][0]=motion_vertical_field_select[0][1] =
      (MPDpicture_structure==BOTTOM_FIELD);
  }

  /* skipped I are spatial-only predicted, */
  /* skipped P and B are temporal-only predicted */
  /* ISO/IEC 13818-2 section 7.7.6: Skipped macroblocks */
  *stwtype = (MPDpicture_coding_type==I_TYPE) ? 8 : 0;

 /* IMPLEMENTATION: clear MACROBLOCK_INTRA */
  *macroblock_type&= ~MACROBLOCK_INTRA;
}

static inline void motion_compensation(int MBA, int macroblock_type, int motion_type,
  int PMV[2][2][2], int motion_vertical_field_select[2][2], int dmvector[2], int stwtype,
  int dct_type)
{
  int bx, by;
  int addflag;

  /* derive current macroblock position within picture */
  /* ISO/IEC 13818-2 section 6.3.1.6 and 6.3.1.7 */
  bx = 16*(MBA%MPDmb_width);
  by = 16*(MBA/MPDmb_width);

  /* motion compensation */
  if (!(macroblock_type & MACROBLOCK_INTRA))
    form_predictions(bx,by,macroblock_type,motion_type,PMV,
      motion_vertical_field_select,dmvector,stwtype);

  /* copy or add block data into picture */
  addflag = (macroblock_type & MACROBLOCK_INTRA)==0;
  Add_Block(0,bx,by,dct_type,addflag);
  Add_Block(1,bx,by,dct_type,addflag);
  Add_Block(2,bx,by,dct_type,addflag);
  Add_Block(3,bx,by,dct_type,addflag);
  Add_Block(4,bx>>1,by>>1,dct_type,addflag);
  Add_Block(5,bx>>1,by>>1,dct_type,addflag);
}

/* decode all macroblocks of the current picture */
/* ISO/IEC 13818-2 section 6.3.16 */
static int slice(int MBAmax)
{
  int MBA;
  int MBAinc, macroblock_type, motion_type, dct_type;
  int dc_dct_pred[3];
  int PMV[2][2][2], motion_vertical_field_select[2][2];
  int dmvector[2];
  int stwtype, stwclass;
  int ret;

  MBA = 0; /* macroblock address */
  MBAinc = 0;

  if((ret=start_of_slice(MBAmax, &MBA, &MBAinc, dc_dct_pred, PMV))!=1)
    return(ret);

  MPDfault_flag=0;

  for (;;)
  {

    /* this is how we properly exit out of picture */
    if (MBA>=MBAmax)
      return(-1); /* all macroblocks decoded */

    MPDld = &MPDbase;

    if (MBAinc==0)
    {
      if (!Show_Bits(23) || MPDfault_flag) /* next_start_code or fault */
      {
resync: /* if Fault_Flag: resynchronize to next next_start_code */
        MPDfault_flag = 0;
        return(0);     /* trigger: go to next slice */
      }
      else /* neither next_start_code nor Fault_Flag */
      {
        /* decode macroblock address increment */
        MBAinc = Get_macroblock_address_increment();

        if (MPDfault_flag) goto resync;
      }
    }

    if (MBA>=MBAmax)
    {
      /* MBAinc points beyond picture dimensions */
      return(-1);
    }

    if (MBAinc==1) /* not skipped */
    {
      ret = decode_macroblock(&macroblock_type, &stwtype, &stwclass,
              &motion_type, &dct_type, PMV, dc_dct_pred,
              motion_vertical_field_select, dmvector);

      if(ret==-1)
        return(-1);

      if(ret==0)
        goto resync;

    }
    else /* MBAinc!=1: skipped macroblock */
    {
      /* ISO/IEC 13818-2 section 7.6.6 */
      skipped_macroblock(dc_dct_pred, PMV, &motion_type,
        motion_vertical_field_select, &stwtype, &macroblock_type);
    }

    /* ISO/IEC 13818-2 section 7.6 */
    motion_compensation(MBA, macroblock_type, motion_type, PMV,
      motion_vertical_field_select, dmvector, stwtype, dct_type);

    /* advance to next macroblock */
    MBA++;
    MBAinc--;

    if (MBA>=MBAmax)
      return(-1); /* all macroblocks decoded */
  }
}

/* a frame's third plane: after the full-size first plane and the quarter-size
   second plane (4:2:0). Inferred: the name and the (height, width, data)
   parameter order are this project's; that order reproduces the original's
   registers for the three third-plane pointers (single-use inline helper,
   accepted 2026-11-12). */
static inline unsigned char *third_plane(int height, int width, unsigned char *data)
{
  return data + width*height + (width/2)*(height/2);
}

/* IMPLEMENTATION: update picture buffer pointers */
static void Update_Picture_Buffers(MPC_CODEC_INTERNAL *codec)
{
  RCMP::FRAME *frame;

  /* B pictures do not need to be save for future reference */
  if (MPDpicture_coding_type==B_TYPE)
  {
    frame = codec->CreateB();
    codec->current = frame;
    codec->output = frame;
  }
  else if (MPDpicture_coding_type==P_TYPE || MPDpicture_coding_type==I_TYPE)
  {
    /* only update at the beginning of the coded frame */
    if (!MPDsecond_field)
    {
      if (codec->forward)
        codec->ReleaseFrame(codec->forward);
      frame = codec->backward;
      codec->forward = frame;
      codec->output = frame;
      frame = codec->CreateIorP();
      codec->backward = frame;
      codec->current = frame;
    }
  }

  frame = codec->backward;
  if (frame)
  {
    MPDbackward_reference_frame[0] = frame->image->Data();
    MPDbackward_reference_frame[1] = frame->image->Data() + frame->image->width*frame->image->height;
    MPDbackward_reference_frame[2] = third_plane(frame->image->height, frame->image->width, frame->image->Data());
  }
  else
  {
    MPDbackward_reference_frame[0] = 0;
    MPDbackward_reference_frame[1] = 0;
    MPDbackward_reference_frame[2] = 0;
  }
  frame = codec->forward;
  if (frame)
  {
    MPDforward_reference_frame[0] = frame->image->Data();
    MPDforward_reference_frame[1] = frame->image->Data() + frame->image->width*frame->image->height;
    MPDforward_reference_frame[2] = third_plane(frame->image->height, frame->image->width, frame->image->Data());
  }
  else
  {
    MPDforward_reference_frame[0] = 0;
    MPDforward_reference_frame[1] = 0;
    MPDforward_reference_frame[2] = 0;
  }
  frame = codec->current;
  if (frame)
  {
    MPDcurrent_frame[0] = frame->image->Data();
    MPDcurrent_frame[1] = frame->image->Data() + frame->image->width*frame->image->height;
    MPDcurrent_frame[2] = third_plane(frame->image->height, frame->image->width, frame->image->Data());
  }
  else
  {
    MPDcurrent_frame[0] = 0;
    MPDcurrent_frame[1] = 0;
    MPDcurrent_frame[2] = 0;
  }

  /* IMPLEMENTATION:
     one-time folding of a line offset into the pointer which stores the
     memory address of the current frame saves offsets and conditional
     branches throughout the remainder of the picture processing loop */
  if (MPDpicture_structure==BOTTOM_FIELD)
  {
    MPDcurrent_frame[0]+= MPDcoded_picture_width;
    MPDcurrent_frame[1]+= MPDchroma_width;
    MPDcurrent_frame[2]+= MPDchroma_width;
  }
}

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
