/* 
 *  PSX-Tools Bundle Pack 
 *  Copyright (C) 1998 Heiko Eissfeldt
 *  portions used& Chris Smith
 *  Modified by Yazoo, then by
 *  Nicolas "Pixel" Noble
 *  
 *  This program is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  This program is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with this program; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yazedc.h"

#include "crctables"


static int encode_L2_Q(unsigned char inout[4 + L2_RAW + 4 + 8 + L2_P + L2_Q])
{
  unsigned char *Q;
  int i,j;

  Q = inout + 4 + L2_RAW + 4 + 8 + L2_P;
  memset(Q, 0, L2_Q);
  for (j = 0; j < 26; j++) {
     for (i = 0; i < 43; i++) {
	unsigned char data;

        /* LSB */
        data = inout[(j*43*2+i*2*44) % (4 + L2_RAW + 4 + 8 + L2_P)];
	if (data != 0) {
		unsigned int base = rs_l12_log[data];

		unsigned int sum = base + DQ[0][i];
		if (sum >= ((1 << RS_L12_BITS)-1))
		  sum -= (1 << RS_L12_BITS)-1;
		
		Q[0]    ^= rs_l12_alog[sum];

		sum = base + DQ[1][i];
		if (sum >= ((1 << RS_L12_BITS)-1))
		  sum -= (1 << RS_L12_BITS)-1;
		
		Q[26*2] ^= rs_l12_alog[sum];
	}
        /* MSB */
        data = inout[(j*43*2+i*2*44+1) % (4 + L2_RAW + 4 + 8 + L2_P)];
	if (data != 0) {
		unsigned int base = rs_l12_log[data];

		unsigned int sum = base+DQ[0][i];
		if (sum >= ((1 << RS_L12_BITS)-1))
		  sum -= (1 << RS_L12_BITS)-1;
		
		Q[1]      ^= rs_l12_alog[sum];

		sum = base + DQ[1][i];
		if (sum >= ((1 << RS_L12_BITS)-1))
		  sum -= (1 << RS_L12_BITS)-1;
		
		Q[26*2+1] ^= rs_l12_alog[sum];
	}
     }
     Q += 2;
  }
  return 0;
}

static int encode_L2_P(unsigned char inout[4 + L2_RAW + 4 + 8 + L2_P])
{
  unsigned char *P;
  int i,j;

  P = inout + 4 + L2_RAW + 4 + 8;
  memset(P, 0, L2_P);
  for (j = 0; j < 43; j++) {
     for (i = 0; i < 24; i++) {
	unsigned char data;

        /* LSB */
        data = inout[i*2*43];
	if (data != 0) {
		unsigned int base = rs_l12_log[data];
		
		unsigned int sum = base + DP[0][i];
		if (sum >= ((1 << RS_L12_BITS)-1))
		  sum -= (1 << RS_L12_BITS)-1;

		P[0]    ^= rs_l12_alog[sum];

		sum = base + DP[1][i];
		if (sum >= ((1 << RS_L12_BITS)-1))
		  sum -= (1 << RS_L12_BITS)-1;
		
		P[43*2] ^= rs_l12_alog[sum];
	}
        /* MSB */
        data = inout[i*2*43+1];
	if (data != 0) {
		unsigned int base = rs_l12_log[data];

		unsigned int sum = base + DP[0][i];
		if (sum >= ((1 << RS_L12_BITS)-1))
		  sum -= (1 << RS_L12_BITS)-1;
		
		P[1]      ^= rs_l12_alog[sum];

		sum = base + DP[1][i];
		if (sum >= ((1 << RS_L12_BITS)-1))
		  sum -= (1 << RS_L12_BITS)-1;
		
		P[43*2+1] ^= rs_l12_alog[sum];
	}
     }
     P += 2;
     inout += 2;
  }
  return 0;
}

#include "crctable.out"
unsigned long int build_edc(unsigned char inout[], int from, int upto)
{
  unsigned char *p = inout+from;
  unsigned long result = 0;

  for (; from <= upto; from++)
    result = EDC_crctable[(result ^ *p++) & 0xffL] ^ (result >> 8);

  return result;
}

int do_encode_L2(unsigned char inout[(12 + 4 + L2_RAW+4+8+L2_Q+L2_P)], int sectortype)
{
  unsigned long int result;

#define SYNCPATTERN "\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"

  /* supply initial sync pattern */
  memcpy(inout, SYNCPATTERN, sizeof(SYNCPATTERN));

  if (sectortype == MODE_0) {
	memset(inout + sizeof(SYNCPATTERN), 0, 4 + L2_RAW + 12 + L2_P + L2_Q);
        //build_address(inout, sectortype, address);
	return 0;
  }
  
  switch (sectortype) {
    case MODE_1:
        //build_address(inout, sectortype, address);
	result = build_edc(inout, 0, 16+2048-1);
	inout[2064+0] = unsigned char(result >> 0L);
	inout[2064+1] = unsigned char(result >> 8L);
	inout[2064+2] = unsigned char(result >> 16L);
	inout[2064+3] = unsigned char(result >> 24L);
	memset(inout+2064+4, 0, 8);
  	encode_L2_P(inout+12);
	encode_L2_Q(inout+12);
	break;
    case MODE_2:
	//build_address(inout, sectortype, address);
	break;
    case MODE_2_FORM_1:
	result = build_edc(inout, 16, 16+8+2048-1);
	inout[2072+0] = unsigned char(result >> 0L);
	inout[2072+1] = unsigned char(result >> 8L);
	inout[2072+2] = unsigned char(result >> 16L);
	inout[2072+3] = unsigned char(result >> 24L);

	/* clear header for P/Q parity calculation */
	inout[12] = 0;
	inout[12+1] = 0;
	inout[12+2] = 0;
	inout[12+3] = 0;
  	encode_L2_P(inout+12);
 	encode_L2_Q(inout+12);
	//build_address(inout, sectortype, address);
	break;
    case MODE_2_FORM_2:
        //build_address(inout, sectortype, address);
	result = build_edc(inout, 16, 16+8+2324-1);
	inout[2348+0] = unsigned char(result >> 0L);
	inout[2348+1] = unsigned char(result >> 8L);
	inout[2348+2] = unsigned char(result >> 16L);
	inout[2348+3] = unsigned char(result >> 24L);
	break;
    default:
	return -1;
  }

  return 0;
}
