/* Adapted from recon.c of the MPEG Software Simulation Group's mpeg2decode
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

// A fragment of recon.cpp (0x80105644): the reference decoder's
// form_component_prediction and its motion compensation entry point
// form_predictions, with form_prediction inlined. Changes from the reference:
// the decoder's globals carry the game's MPD names, chroma is fixed at 4:2:0
// (the game has no chroma_format), the invalid-motion-type messages are
// absent, and the definitions are ordered so that form_prediction precedes its
// use. In form_component_prediction the game's non-averaging paths are written
// out for 16- and 8-pixel rows (w is 16 or 8), the vertical half-pel paths read
// the row below through a second pointer, and the two-tap results are narrowed
// to bytes; the exact statement forms (including the cast in the 8-pixel copy)
// are inferred from the code. The globals' array types and the constants are
// taken from the reference (global.h).
#define MACROBLOCK_MOTION_FORWARD 8
#define MACROBLOCK_MOTION_BACKWARD 4
#define P_TYPE 2
#define TOP_FIELD 1
#define BOTTOM_FIELD 2
#define FRAME_PICTURE 3
#define MC_FIELD 1
#define MC_FRAME 2
#define MC_16X8 2
#define MC_DMV 3

extern unsigned char* MPDforward_reference_frame[3];
extern unsigned char* MPDbackward_reference_frame[3];
extern unsigned char* MPDcurrent_frame[3];
extern int MPDcoded_picture_width;
extern int MPDsecond_field;
extern int MPDpicture_coding_type;
extern int MPDpicture_structure;

void Dual_Prime_Arithmetic(int DMV[][2], int* dmvector, int mvx, int mvy);

static void form_component_prediction(unsigned char* src, unsigned char* dst, int lx, int lx2, int w, int h, int x, int y, int dx, int dy, int average_flag)
{
  int xint, xh, yint, yh;
  int i, j, v;
  unsigned char *s, *s2, *d;

  /* half pel scaling for integer vectors */
  xint = dx>>1;
  xh = dx & 1;
  yint = dy>>1;
  yh = dy & 1;

  /* compute the linear address of pel_ref[][] and pel_pred[][]
     based on cartesian/raster cordinates provided */
  s = src + lx*(y+yint) + x + xint;
  d = dst + lx*y + x;

  if (!xh && !yh) /* no horizontal nor vertical half-pel */
  {
    if (average_flag)
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          v = d[i]+s[i];
          d[i] = (v+(v>=0?1:0))>>1;
        }

        s+= lx2;
        d+= lx2;
      }
    }
    else if (w == 16)
    {
      for (j=0; j<h; j++)
      {
        d[0] = s[0];
        d[1] = s[1];
        d[2] = s[2];
        d[3] = s[3];
        d[4] = s[4];
        d[5] = s[5];
        d[6] = s[6];
        d[7] = s[7];
        d[8] = s[8];
        d[9] = s[9];
        d[10] = s[10];
        d[11] = s[11];
        d[12] = s[12];
        d[13] = s[13];
        d[14] = s[14];
        d[15] = s[15];

        s+= lx2;
        d+= lx2;
      }
    }
    else
    {
      for (j=0; j<h; j++)
      {
        d[0] = (unsigned int)s[0];
        d[1] = (unsigned int)s[1];
        d[2] = (unsigned int)s[2];
        d[3] = (unsigned int)s[3];
        d[4] = (unsigned int)s[4];
        d[5] = (unsigned int)s[5];
        d[6] = (unsigned int)s[6];
        d[7] = (unsigned int)s[7];

        s+= lx2;
        d+= lx2;
      }
    }
  }
  else if (!xh && yh) /* no horizontal but vertical half-pel */
  {
    if (average_flag)
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          v = d[i] + (unsigned char)((unsigned int)(s[i]+s[i+lx]+1)>>1);
          d[i] = (v+(v>=0?1:0))>>1;
        }

        s+= lx2;
        d+= lx2;
      }
    }
    else
    {
      s2 = s + lx;
      if (w == 16)
      {
        for (j=0; j<h; j++)
        {
          d[0] = (unsigned char)((unsigned int)(s[0]+s2[0]+1)>>1);
          d[1] = (unsigned char)((unsigned int)(s[1]+s2[1]+1)>>1);
          d[2] = (unsigned char)((unsigned int)(s[2]+s2[2]+1)>>1);
          d[3] = (unsigned char)((unsigned int)(s[3]+s2[3]+1)>>1);
          d[4] = (unsigned char)((unsigned int)(s[4]+s2[4]+1)>>1);
          d[5] = (unsigned char)((unsigned int)(s[5]+s2[5]+1)>>1);
          d[6] = (unsigned char)((unsigned int)(s[6]+s2[6]+1)>>1);
          d[7] = (unsigned char)((unsigned int)(s[7]+s2[7]+1)>>1);
          d[8] = (unsigned char)((unsigned int)(s[8]+s2[8]+1)>>1);
          d[9] = (unsigned char)((unsigned int)(s[9]+s2[9]+1)>>1);
          d[10] = (unsigned char)((unsigned int)(s[10]+s2[10]+1)>>1);
          d[11] = (unsigned char)((unsigned int)(s[11]+s2[11]+1)>>1);
          d[12] = (unsigned char)((unsigned int)(s[12]+s2[12]+1)>>1);
          d[13] = (unsigned char)((unsigned int)(s[13]+s2[13]+1)>>1);
          d[14] = (unsigned char)((unsigned int)(s[14]+s2[14]+1)>>1);
          d[15] = (unsigned char)((unsigned int)(s[15]+s2[15]+1)>>1);

          s+= lx2;
          s2+= lx2;
          d+= lx2;
        }
      }
      else
      {
        for (j=0; j<h; j++)
        {
          d[0] = (unsigned char)((unsigned int)(s[0]+s2[0]+1)>>1);
          d[1] = (unsigned char)((unsigned int)(s[1]+s2[1]+1)>>1);
          d[2] = (unsigned char)((unsigned int)(s[2]+s2[2]+1)>>1);
          d[3] = (unsigned char)((unsigned int)(s[3]+s2[3]+1)>>1);
          d[4] = (unsigned char)((unsigned int)(s[4]+s2[4]+1)>>1);
          d[5] = (unsigned char)((unsigned int)(s[5]+s2[5]+1)>>1);
          d[6] = (unsigned char)((unsigned int)(s[6]+s2[6]+1)>>1);
          d[7] = (unsigned char)((unsigned int)(s[7]+s2[7]+1)>>1);

          s+= lx2;
          s2+= lx2;
          d+= lx2;
        }
      }
    }
  }
  else if (xh && !yh) /* horizontal but no vertical half-pel */
  {
    if (average_flag)
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          v = d[i] + (unsigned char)((unsigned int)(s[i]+s[i+1]+1)>>1);
          d[i] = (v+(v>=0?1:0))>>1;
        }

        s+= lx2;
        d+= lx2;
      }
    }
    else if (w == 16)
    {
      for (j=0; j<h; j++)
      {
        d[0] = (unsigned char)((unsigned int)(s[0]+s[1]+1)>>1);
        d[1] = (unsigned char)((unsigned int)(s[1]+s[2]+1)>>1);
        d[2] = (unsigned char)((unsigned int)(s[2]+s[3]+1)>>1);
        d[3] = (unsigned char)((unsigned int)(s[3]+s[4]+1)>>1);
        d[4] = (unsigned char)((unsigned int)(s[4]+s[5]+1)>>1);
        d[5] = (unsigned char)((unsigned int)(s[5]+s[6]+1)>>1);
        d[6] = (unsigned char)((unsigned int)(s[6]+s[7]+1)>>1);
        d[7] = (unsigned char)((unsigned int)(s[7]+s[8]+1)>>1);
        d[8] = (unsigned char)((unsigned int)(s[8]+s[9]+1)>>1);
        d[9] = (unsigned char)((unsigned int)(s[9]+s[10]+1)>>1);
        d[10] = (unsigned char)((unsigned int)(s[10]+s[11]+1)>>1);
        d[11] = (unsigned char)((unsigned int)(s[11]+s[12]+1)>>1);
        d[12] = (unsigned char)((unsigned int)(s[12]+s[13]+1)>>1);
        d[13] = (unsigned char)((unsigned int)(s[13]+s[14]+1)>>1);
        d[14] = (unsigned char)((unsigned int)(s[14]+s[15]+1)>>1);
        d[15] = (unsigned char)((unsigned int)(s[15]+s[16]+1)>>1);

        s+= lx2;
        d+= lx2;
      }
    }
    else
    {
      for (j=0; j<h; j++)
      {
        d[0] = (unsigned char)((unsigned int)(s[0]+s[1]+1)>>1);
        d[1] = (unsigned char)((unsigned int)(s[1]+s[2]+1)>>1);
        d[2] = (unsigned char)((unsigned int)(s[2]+s[3]+1)>>1);
        d[3] = (unsigned char)((unsigned int)(s[3]+s[4]+1)>>1);
        d[4] = (unsigned char)((unsigned int)(s[4]+s[5]+1)>>1);
        d[5] = (unsigned char)((unsigned int)(s[5]+s[6]+1)>>1);
        d[6] = (unsigned char)((unsigned int)(s[6]+s[7]+1)>>1);
        d[7] = (unsigned char)((unsigned int)(s[7]+s[8]+1)>>1);

        s+= lx2;
        d+= lx2;
      }
    }
  }
  else /* if (xh && yh) horizontal and vertical half-pel */
  {
    if (average_flag)
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          v = d[i] + ((unsigned int)(s[i]+s[i+1]+s[i+lx]+s[i+lx+1]+2)>>2);
          d[i] = (v+(v>=0?1:0))>>1;
        }

        s+= lx2;
        d+= lx2;
      }
    }
    else
    {
      s2 = s + lx;
      if (w == 16)
      {
        for (j=0; j<h; j++)
        {
          d[0] = (unsigned char)((unsigned int)(s[0]+s[1]+s2[0]+s2[1]+2)>>2);
          d[1] = (unsigned char)((unsigned int)(s[1]+s[2]+s2[1]+s2[2]+2)>>2);
          d[2] = (unsigned char)((unsigned int)(s[2]+s[3]+s2[2]+s2[3]+2)>>2);
          d[3] = (unsigned char)((unsigned int)(s[3]+s[4]+s2[3]+s2[4]+2)>>2);
          d[4] = (unsigned char)((unsigned int)(s[4]+s[5]+s2[4]+s2[5]+2)>>2);
          d[5] = (unsigned char)((unsigned int)(s[5]+s[6]+s2[5]+s2[6]+2)>>2);
          d[6] = (unsigned char)((unsigned int)(s[6]+s[7]+s2[6]+s2[7]+2)>>2);
          d[7] = (unsigned char)((unsigned int)(s[7]+s[8]+s2[7]+s2[8]+2)>>2);
          d[8] = (unsigned char)((unsigned int)(s[8]+s[9]+s2[8]+s2[9]+2)>>2);
          d[9] = (unsigned char)((unsigned int)(s[9]+s[10]+s2[9]+s2[10]+2)>>2);
          d[10] = (unsigned char)((unsigned int)(s[10]+s[11]+s2[10]+s2[11]+2)>>2);
          d[11] = (unsigned char)((unsigned int)(s[11]+s[12]+s2[11]+s2[12]+2)>>2);
          d[12] = (unsigned char)((unsigned int)(s[12]+s[13]+s2[12]+s2[13]+2)>>2);
          d[13] = (unsigned char)((unsigned int)(s[13]+s[14]+s2[13]+s2[14]+2)>>2);
          d[14] = (unsigned char)((unsigned int)(s[14]+s[15]+s2[14]+s2[15]+2)>>2);
          d[15] = (unsigned char)((unsigned int)(s[15]+s[16]+s2[15]+s2[16]+2)>>2);

          s+= lx2;
          s2+= lx2;
          d+= lx2;
        }
      }
      else
      {
        for (j=0; j<h; j++)
        {
          d[0] = (unsigned char)((unsigned int)(s[0]+s[1]+s2[0]+s2[1]+2)>>2);
          d[1] = (unsigned char)((unsigned int)(s[1]+s[2]+s2[1]+s2[2]+2)>>2);
          d[2] = (unsigned char)((unsigned int)(s[2]+s[3]+s2[2]+s2[3]+2)>>2);
          d[3] = (unsigned char)((unsigned int)(s[3]+s[4]+s2[3]+s2[4]+2)>>2);
          d[4] = (unsigned char)((unsigned int)(s[4]+s[5]+s2[4]+s2[5]+2)>>2);
          d[5] = (unsigned char)((unsigned int)(s[5]+s[6]+s2[5]+s2[6]+2)>>2);
          d[6] = (unsigned char)((unsigned int)(s[6]+s[7]+s2[6]+s2[7]+2)>>2);
          d[7] = (unsigned char)((unsigned int)(s[7]+s[8]+s2[7]+s2[8]+2)>>2);

          s+= lx2;
          s2+= lx2;
          d+= lx2;
        }
      }
    }
  }
}


static inline void form_prediction(unsigned char* src[], int sfield, unsigned char* dst[], int dfield, int lx, int lx2, int w, int h, int x, int y, int dx, int dy, int average_flag)
{
  /* Y */
  form_component_prediction(src[0]+(sfield?lx2>>1:0),dst[0]+(dfield?lx2>>1:0),
    lx,lx2,w,h,x,y,dx,dy,average_flag);

  lx>>=1; lx2>>=1; w>>=1; x>>=1; dx/=2;
  h>>=1; y>>=1; dy/=2;

  /* Cb */
  form_component_prediction(src[1]+(sfield?lx2>>1:0),dst[1]+(dfield?lx2>>1:0),
    lx,lx2,w,h,x,y,dx,dy,average_flag);

  /* Cr */
  form_component_prediction(src[2]+(sfield?lx2>>1:0),dst[2]+(dfield?lx2>>1:0),
    lx,lx2,w,h,x,y,dx,dy,average_flag);
}

void form_predictions(int bx, int by, int macroblock_type, int motion_type, int PMV[2][2][2], int motion_vertical_field_select[2][2], int dmvector[2], int stwtype)
{
  int currentfield;
  unsigned char **predframe;
  int DMV[2][2];
  int stwtop, stwbot;

  stwtop = stwtype%3; /* 0:temporal, 1:(spat+temp)/2, 2:spatial */
  stwbot = stwtype/3;

  if ((macroblock_type & MACROBLOCK_MOTION_FORWARD) 
   || (MPDpicture_coding_type==P_TYPE))
  {
    if (MPDpicture_structure==FRAME_PICTURE)
    {
      if ((motion_type==MC_FRAME) 
        || !(macroblock_type & MACROBLOCK_MOTION_FORWARD))
      {
        /* frame-based prediction (broken into top and bottom halves
             for spatial scalability prediction purposes) */
        if (stwtop<2)
          form_prediction(MPDforward_reference_frame,0,MPDcurrent_frame,0,
            MPDcoded_picture_width,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwtop);

        if (stwbot<2)
          form_prediction(MPDforward_reference_frame,1,MPDcurrent_frame,1,
            MPDcoded_picture_width,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwbot);
      }
      else if (motion_type==MC_FIELD) /* field-based prediction */
      {
        /* top field prediction */
        if (stwtop<2)
          form_prediction(MPDforward_reference_frame,motion_vertical_field_select[0][0],
            MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
            bx,by>>1,PMV[0][0][0],PMV[0][0][1]>>1,stwtop);

        /* bottom field prediction */
        if (stwbot<2)
          form_prediction(MPDforward_reference_frame,motion_vertical_field_select[1][0],
            MPDcurrent_frame,1,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
            bx,by>>1,PMV[1][0][0],PMV[1][0][1]>>1,stwbot);
      }
      else if (motion_type==MC_DMV) /* dual prime prediction */
      {
        /* calculate derived motion vectors */
        Dual_Prime_Arithmetic(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]>>1);

        if (stwtop<2)
        {
          /* predict top field from top field */
          form_prediction(MPDforward_reference_frame,0,MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by>>1,
            PMV[0][0][0],PMV[0][0][1]>>1,0);

          /* predict and add to top field from bottom field */
          form_prediction(MPDforward_reference_frame,1,MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by>>1,
            DMV[0][0],DMV[0][1],1);
        }

        if (stwbot<2)
        {
          /* predict bottom field from bottom field */
          form_prediction(MPDforward_reference_frame,1,MPDcurrent_frame,1,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by>>1,
            PMV[0][0][0],PMV[0][0][1]>>1,0);

          /* predict and add to bottom field from top field */
          form_prediction(MPDforward_reference_frame,0,MPDcurrent_frame,1,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by>>1,
            DMV[1][0],DMV[1][1],1);
        }
      }
    }
    else /* TOP_FIELD or BOTTOM_FIELD */
    {
      /* field picture */
      currentfield = (MPDpicture_structure==BOTTOM_FIELD);

      /* determine which frame to use for prediction */
      if ((MPDpicture_coding_type==P_TYPE) && MPDsecond_field
         && (currentfield!=motion_vertical_field_select[0][0]))
        predframe = MPDbackward_reference_frame; /* same frame */
      else
        predframe = MPDforward_reference_frame; /* previous frame */

      if ((motion_type==MC_FIELD)
        || !(macroblock_type & MACROBLOCK_MOTION_FORWARD))
      {
        /* field-based prediction */
        if (stwtop<2)
          form_prediction(predframe,motion_vertical_field_select[0][0],MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,16,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwtop);
      }
      else if (motion_type==MC_16X8)
      {
        if (stwtop<2)
        {
          form_prediction(predframe,motion_vertical_field_select[0][0],MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwtop);

          /* determine which frame to use for lower half prediction */
          if ((MPDpicture_coding_type==P_TYPE) && MPDsecond_field
             && (currentfield!=motion_vertical_field_select[1][0]))
            predframe = MPDbackward_reference_frame; /* same frame */
          else
            predframe = MPDforward_reference_frame; /* previous frame */

          form_prediction(predframe,motion_vertical_field_select[1][0],MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by+8,
            PMV[1][0][0],PMV[1][0][1],stwtop);
        }
      }
      else if (motion_type==MC_DMV) /* dual prime prediction */
      {
        if (MPDsecond_field)
          predframe = MPDbackward_reference_frame; /* same frame */
        else
          predframe = MPDforward_reference_frame; /* previous frame */

        /* calculate derived motion vectors */
        Dual_Prime_Arithmetic(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]);

        /* predict from field of same parity */
        form_prediction(MPDforward_reference_frame,currentfield,MPDcurrent_frame,0,
          MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,16,bx,by,
          PMV[0][0][0],PMV[0][0][1],0);

        /* predict from field of opposite parity */
        form_prediction(predframe,!currentfield,MPDcurrent_frame,0,
          MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,16,bx,by,
          DMV[0][0],DMV[0][1],1);
      }
    }
    stwtop = stwbot = 1;
  }

  if (macroblock_type & MACROBLOCK_MOTION_BACKWARD)
  {
    if (MPDpicture_structure==FRAME_PICTURE)
    {
      if (motion_type==MC_FRAME)
      {
        /* frame-based prediction */
        if (stwtop<2)
          form_prediction(MPDbackward_reference_frame,0,MPDcurrent_frame,0,
            MPDcoded_picture_width,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][1][0],PMV[0][1][1],stwtop);

        if (stwbot<2)
          form_prediction(MPDbackward_reference_frame,1,MPDcurrent_frame,1,
            MPDcoded_picture_width,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][1][0],PMV[0][1][1],stwbot);
      }
      else /* field-based prediction */
      {
        /* top field prediction */
        if (stwtop<2)
          form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[0][1],
            MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
            bx,by>>1,PMV[0][1][0],PMV[0][1][1]>>1,stwtop);

        /* bottom field prediction */
        if (stwbot<2)
          form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[1][1],
            MPDcurrent_frame,1,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
            bx,by>>1,PMV[1][1][0],PMV[1][1][1]>>1,stwbot);
      }
    }
    else /* TOP_FIELD or BOTTOM_FIELD */
    {
      /* field picture */
      if (motion_type==MC_FIELD)
      {
        /* field-based prediction */
        form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[0][1],
          MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,16,
          bx,by,PMV[0][1][0],PMV[0][1][1],stwtop);
      }
      else if (motion_type==MC_16X8)
      {
        form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[0][1],
          MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
          bx,by,PMV[0][1][0],PMV[0][1][1],stwtop);

        form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[1][1],
          MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
          bx,by+8,PMV[1][1][0],PMV[1][1][1],stwtop);
      }
    }
  }
}
