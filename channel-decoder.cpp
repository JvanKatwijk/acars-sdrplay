#
/*
 *  Copyright (c) 2015 Thierry Leconte
 *
 *   
 *   This code is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *	Copyright (c) 2018 
 *	Jan Van Katwijk, Lazy Chair Computing
 *	This file is a rewrite of the file "msk.c" and
 *	parts of acars.c of the original acars software
 *	
 */
#include	"channel-decoder.h"
#include	<cstring>
#include	"syndrom.h"
#include	"printer.h"

	channelDecoder::channelDecoder (printer	*myPrinter,
	                                int channelNumber,
	                                bool verbose) {

	this	-> myPrinter	= myPrinter;
	channel		= channelNumber;
	this	-> verbose	= verbose;
	fprintf (stderr, "hier is verbose = %d\n", verbose);
	MskPhi		= 0;
	MskS		= 0;
	MskDf		= 0;
	MskClk		= 0;
	Msklvl		= 0;
	idx		= 0;
	memset (inbuffer, 0, FLEN * sizeof (std::complex<float>));

	for (int i = 0; i < FLEN; i ++)
	   matchedFilter_h [i] = 
	      matchedFilter_h [FLEN + i] =
	                  cosf (2.0 * M_PI * 600.0 / WORKING_RATE * (i - FLEN/2));

	nbits		= 0;
	outbits		= 0;
	Acarsstate	= WSYN;
	blk_len		= 0;
	blk_err		= 0;
}

	channelDecoder::~channelDecoder	(void) {
}


void	channelDecoder:: demodMSK (float *dm_buffer, int len) {
int	i;

	for (i = 0; i < len; i++) {
           float in;
           double s;
           std::complex<float> v;
           int j, o;

//	VCO 
	   s = 1800.0 / WORKING_RATE * 2.0 * M_PI + MskDf;
           MskPhi += s;
           if (MskPhi >= 2.0 * M_PI)
              MskPhi -= 2.0 * M_PI;
	   if (MskPhi < 0.0)
	      MskPhi += 2.0 * M_PI;

//	mixer 
           in =  dm_buffer [i];	// 
	   inbuffer [idx] =  std::complex<float> (
	                                 in * cos (MskPhi),
	                                 in * -sin (MskPhi));
           idx = (idx + 1) % FLEN;

//	bit clock 
	   MskClk += s;
	   if (MskClk >= 3 * M_PI / 2.0) {
	      double dphi;
	      float vo, lvl;

	      MskClk -= 3 * M_PI / 2.0;
//	matched filter 
	      o = FLEN - idx;
	      v = std::complex<float> (0, 0);
	      for (j = 0; j < FLEN; j++, o++) {
	         v += std::complex<float> (
	             matchedFilter_h [o] * real (inbuffer [j]),
	             matchedFilter_h [o] * imag (inbuffer [j]));
	      }

//	normalize 
	      lvl = abs (v);
	      v =  std::complex<float> (real (v) / (lvl + 1e-6),
	                                imag (v) / (lvl + 1e-6));
	      Msklvl = 0.99 * Msklvl + 0.01 * lvl / 5.2;

	      switch (MskS & 3) {
	         case 0:
	            vo = real (v);
	            putbit (vo);
	            dphi = vo >= 0 ? imag (v) : -imag (v);
	            break;

	         case 1:
	            vo = imag (v);
	            putbit (vo);
	            dphi = vo >= 0 ? -real (v) : real (v);
	            break;

	         case 2:
	            vo = real (v);
	            putbit (-vo);
	            dphi = vo >= 0 ? imag (v) : -imag (v);
	            break;

	         case 3:
	            vo = imag (v);
	            putbit (-vo);
	            dphi = vo >= 0 ? -real (v) : real (v);
	            break;
	      }
              MskS++;
//	PLL filter */
              MskDf = PLLC * dphi;
	   }
	}
}


void	channelDecoder::putbit (float v) {

	outbits >>= 1;
	if (v > 0) {
	   outbits |= 0x80;
	} 

	nbits--;
	if (nbits <= 0)
	   decodeAcars (outbits);
}

void	channelDecoder::decodeAcars (uint8_t theByte) {
	switch (Acarsstate) {
	   case WSYN:
	      if (theByte == SYN) {
	         Acarsstate = SYN2;
	         nbits = 8;
	         return;
	      }
	      if (theByte == (uint8_t)~SYN) {
	         MskS ^= 2;
	         Acarsstate = SYN2;
	         nbits = 8;
	         return;
	      }
	      nbits = 1;
	      return;

	   case SYN2:
	      if (theByte == SYN) {
	         Acarsstate = SOH1;
	         nbits = 8;
	         return;
	      }	
	      if (theByte == (uint8_t)~SYN) {
	         MskS ^= 2;
	         nbits = 8;
	         return;
	      }

	      Acarsstate = WSYN;
	      MskDf = 0;
	      nbits = 1;
	      return;

	   case SOH1:	
	      if (theByte == SOH) {
	         gettimeofday (&(blk_tm), NULL);
	         Acarsstate = TXT;
	         blk_len = 0;
	         blk_err = 0;
	         nbits = 8;
	         Msklvl = 0;
	         return;
	      }

	      Acarsstate = WSYN;
	      MskDf = 0;
	      nbits = 1;
	      return;

	   case TXT:
	      blk_txt [blk_len ++] = theByte;
	      if ((numbits [theByte] & 1) == 0) {
	         blk_err++;
	         if (blk_err > MAXPERR + 1) {
	            if (verbose)
	               fprintf (stderr,
	                        "#%d too many parity errors\n", channel);
	            Acarsstate = WSYN;
	            MskDf = 0;
	            nbits = 1;
	            return;
	         }
	      }
	      if ((theByte == ETX) || (theByte == ETB)) {
	         Acarsstate = CRC1;
	         nbits = 8;
	         return;
	      }
	      if ((blk_len > 20) && (theByte == DLE)) {
	         if (verbose)
	            fprintf (stderr, "#%d miss txt end\n", channel);
	         blk_len -= 3;
	         blk_crc [0] = blk_txt [blk_len];
	         blk_crc [1] = blk_txt [blk_len + 1];
	         Acarsstate = CRC2;
	         goto putmsg_lbl;
	      }

	      if (blk_len > 240) {
	         if (verbose) fprintf (stderr,
	                     "#%d too long\n", channel);
	         Acarsstate = WSYN;
	         MskDf = 0;
	         nbits = 1;
	         return;
	      }
	      nbits = 8;
	      return;

	   case CRC1:
	      blk_crc [0] = theByte;
	      Acarsstate = CRC2;
	      nbits = 8;
	      return;

	   case CRC2:
	      blk_crc [1] = theByte;
 putmsg_lbl:
//	      blk_lvl = 10 * log10 (Msklvl);
	      Acarsstate = END;
	      nbits = 8;
	      processBlock (blk_crc, blk_txt, blk_len);
	      return;

	   case END:
	      Acarsstate = WSYN;
	      MskDf = 0;
	      nbits = 8;
	      return;
	}
}


int	channelDecoder::fixprerr (uint8_t *blk_txt, int blk_len,
	                          const uint16_t crc, int *pr, int pn) {
int i;

	if (pn > 0) {
/*	try to recursively fix parity error */
	   for (i = 0; i < 8; i++) {
	      if (fixprerr (blk_txt, blk_len,
	                    crc ^ syndrom[i + 8 * (blk_len - *pr + 1)],
	                    pr + 1, pn - 1)) {
	         blk_txt [*pr] ^= (1 << i);
	         return 1;
	      }
	   }
	   return 0;
	} else {
/*	end of recursion : no more parity error */
	   if (crc == 0)
	      return 1;

/*	test remainding error in crc */
	   for (i = 0; i < 2 * 8; i++)
	      if (syndrom [i] == crc) 
	         return 1;
	   return 0;
	}
}

int	channelDecoder::fixdberr (uint8_t *blk_txt,
	                          int blk_len, const uint16_t crc) {
int i,j,k;

/*	test remaining error in crc */
	for (i = 0; i < 2 * 8; i++)
	   if (syndrom [i] == crc) 
	      return 1;

/*	test double error in bytes */
	for (k = 0; k < blk_len ; k++) {
	   int bo = 8 * (blk_len - k + 1);
	   for (i = 0; i < 8; i++) {
	      for (j = 0; j < 8; j++) {
	         if (i == j)
	            continue;
	         if ((crc ^ syndrom [i + bo] ^ syndrom [j + bo])==0) {
	            blk_txt [k] ^= (1 << i);
	            blk_txt [k] ^= (1 << j);
	            return 1;
	         }
	      }
	   }
	}
	return 0;
}


void	channelDecoder::processBlock (uint8_t *blk_crc,
	                              uint8_t *blk_txt, int blk_len) {
int	pn, i;
int	pr [MAXPERR];
uint16_t the_crc;

	if (blk_len < 13) 
	   return;

/*      force STX/ETX */
	blk_txt [12] &= (ETX | STX);
	blk_txt [12] |= (ETX & STX);

//      parity check */
	pn = 0;
	for (i = 0; i < blk_len; i++) {
	   if ((numbits [blk_txt [i]] & 1) == 0) {
	      if (pn < MAXPERR)
	         pr [pn] = i;
	      pn ++;
	      if (pn > MAXPERR) {
	         if (verbose)
	               fprintf (stderr, "#Channel %d: too many parity errors: %d\n", channel, pn);
	         return;
	      }
	   }
	}

//      crc check */
	the_crc = 0;
	for (i = 0; i < blk_len; i++)
	   update_crc (the_crc, blk_txt [i]);

	update_crc (the_crc, blk_crc [0]);
	update_crc (the_crc, blk_crc [1]);
	if ((the_crc != 0) && verbose)
	   fprintf (stderr, "#%d: crc error\n", channel);

/*      try to fix error */
	if (pn > 0) {
	   if (fixprerr (blk_txt, blk_len, the_crc, pr, pn) == 0) {
	      if (verbose)
	         fprintf (stderr, "#%d not able to fix errors\n", channel);
	      return;
	   }
	   else
	   if (verbose)
	      fprintf(stderr, "#%d errors fixed\n", channel);
	   else {
	      if (the_crc != 0) {
	         if (fixdberr (blk_txt, blk_len, the_crc) == 0) {
	            if (verbose)
	               fprintf (stderr, "#%d not able to fix errors\n", channel);
	            return;
	         }
	      }
	      else
	      if (verbose)
	         fprintf (stderr, "#%d: errors fixed\n", channel);
	   }
	}

/*	redo parity checking and removing */
	pn = 0;
	for (i = 0; i < blk_len; i++) {
	   if ((numbits [blk_txt [i]] & 1) == 0) 
	      pn ++;

	   blk_txt [i] &= 0x7f;
	}

	if (pn > 0) {
	   fprintf (stderr, "#%d: parity check problem\n", channel);
	   return;
	}

	myPrinter -> output_msg (channel, blk_txt, blk_len, blk_tm);
}

