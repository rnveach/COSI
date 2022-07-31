/*  
 *  PSX-Tools Bundle Pack  
 *  Copyright (C) 1998 Heiko Eissfeldt 
 *  portions used& Chris Smith 
 *  First modified by Yazoo, then by 
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

#ifndef __YAZEDC_H__

//#include "Exceptions.h"

#define RS_L12_BITS 8

/* audio sector definitions for CIRC */
//#define FRAMES_PER_SECTOR 98
/* user data bytes per frame */
//#define L1_RAW 24
/* parity bytes with 8 bit */
//#define L1_Q   4
//#define L1_P   4

/* data sector definitions for RSPC */
/* user data bytes per frame */
#define L2_RAW (1024*2)
/* parity bytes for 16 bit units */
#define L2_Q   (26*2*2)
#define L2_P   (43*2*2)

/* known sector types */
#define MODE_0	0
#define MODE_1	1
#define MODE_2	2
#define MODE_2_FORM_1	3
#define MODE_2_FORM_2	4

/* r-w sub channel definitions */
//#define RS_SUB_RW_BITS 6

//#define PACKETS_PER_SUBCHANNELFRAME 4
//#define LSUB_RAW 18
//#define LSUB_QRAW 2
/* 6 bit */
//#define LSUB_Q 2
//#define LSUB_P 4

int do_encode_L2(unsigned char *inout, int sectortype);

#endif
