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
 *	Jan van Katwijk, Lazy Chair Computing
 *	This file is based on and a rewrite of acars.c and output.c
 *	of the original acars software of Thierry Leconte
 */
#include	"printer.h"
#include	<cstring>
#include	"label.c"

static flight_t  *flight_head = NULL;

	printer::printer (int slots, int outtype, int channels):
	                                           freeSlots (slots) {
	this	-> outtype	= outtype;
	this	-> channels	= channels;
	nextIn                  = 0;
	nextOut                 = 0;
}

	printer::~printer (void) {
	if (running. load ()) {
	   running. store (false);
	   threadHandle. join ();
	}
}

void	printer::start	(void) {
        running. store (true);
        threadHandle = std::thread (&printer::run, this);
}

void	printer::stop	(void) {
	if (running. load ()) {
	   running. store (false);
	   threadHandle. join ();
	}
}

void	printer::output_msg (int channel, uint8_t *blk_txt,
	                     int16_t blk_len, struct timeval blk_tm) {
	while (!freeSlots. tryAcquire (200))
	   if (!running. load ())
	      return;

	memcpy (&theData [nextIn]. blk_txt, blk_txt, blk_len);
	theData [nextIn]. blk_len = blk_len;
	theData [nextIn]. blk_tm  = blk_tm;
	theData [nextIn]. channel	= channel;
        nextIn = (nextIn + 1) % 20;
        usedSlots. Release ();
}

void	printer::run (void) {
	fprintf (stderr, "printer is running\n");
	running. store (true);
	while (running. load ()) {
	   while (!usedSlots. tryAcquire (200))
	      if (!running)
	         return;
           process_msg (theData [nextOut]. channel,
	                theData [nextOut]. blk_txt,
	                theData [nextOut]. blk_len, theData [nextOut]. blk_tm);
	   nextOut = (nextOut + 1) % 20;
	   freeSlots. Release ();
        }
}

void	printer::process_msg (int channel, uint8_t *blk_txt,
	                         int16_t blk_len, struct timeval blk_tm) {
acarsmsg_t msg;
int	i, k;
bool	messageFlag	= false;
	msg. channel		= channel;
	msg. messageTime	= blk_tm;
	msg. mode		= blk_txt [k++];
	for (i = 0; i < 7; i ++, k ++) 
	   msg. addr [i] = blk_txt [k];
	msg. addr [7] = 0;
//
//	ACK/NAK
	msg. ack	= blk_txt [k++];

	msg. label [0]	= blk_txt [k++];
	msg. label [1]	= blk_txt [k++];
	if (msg.label [1] == 0x7f)
	   msg. label [1] ='d';
	msg. label [2] = '\0';

        msg. bid	= blk_txt [k++];

	/* txt start  */
        msg. bs		= blk_txt [k++];

        msg. no  [0]	= '\0';
        msg. fid [0]	= '\0';
        msg. txt [0]	= '\0';

        if ((msg. bs == 0x03 || msg. mode > 'Z') && false)
	   return;

	if (msg.bs != 0x03) {
	   if ((msg.mode <= 'Z') && (msg. bid <= '9')) {
//	message no 
	      for (i = 0; i < 4 && k < blk_len - 1; i++, k++) 
	         msg. no [i] = blk_txt [k];

	      msg. no [i] = '\0';

//	Flight id 
	      for (i = 0; i < 6 && k < blk_len - 1; i++, k++) 
	         msg. fid [i] = blk_txt [k];

	      msg. fid [i] = '\0';
	      messageFlag	= true;
	   }

//	 Message txt 
	   for (i = 0; k < blk_len - 1; i++, k++)
	      msg. txt [i] = blk_txt [k];
	   msg. txt [i] = 0;
        }
//	txt end 
	msg. be = blk_txt [blk_len - 1];

	if ((outtype == OUTTYPE_MONITOR) && messageFlag)
	   addFlight (&msg, channel);

	switch (outtype) {
	   default:
	   case OUTTYPE_NONE:
	      break;

	   case OUTTYPE_ONELINE:
	      printoneline (&msg, channel);
	      break;

	   case OUTTYPE_STD:
	      printmsg (&msg);
	      break;
	
	   case OUTTYPE_MONITOR:
	      
	      printmonitor (&msg, channel);
	      break;
	}
}


void	printer::printmsg (acarsmsg_t *msg) {
	printdate (msg -> messageTime);
	fprintf (stderr, "\n channel %d --------------------------------\n",
	                                                msg -> channel);
	fprintf (stderr, "Mode : %1c ", msg -> mode);
	fprintf (stderr, "Label : %2s ", msg -> label);
	if (msg -> bid) {
	   fprintf (stderr, "Id : %1c ", msg -> bid);
	   if (msg -> ack==0x15)
	      fprintf (stderr, "Nak\n");
	   else
	      fprintf (stderr, "Ack : %1c\n", msg -> ack);
	   fprintf (stderr, "Aircraft reg: %s ", msg -> addr);
	   if (msg->mode <= 'Z') {
	      fprintf (stderr, "Flight id: %s\n", msg -> fid);
	      fprintf (stderr, "No: %4s", msg -> no);
	   }
	}

	fprintf(stderr, "\n");
	if (msg -> txt [0])
	   fprintf (stderr, "%s\n", msg -> txt);
	if (msg -> be == 0x17)
	   fprintf (stderr, "ETB\n");

	fflush(stderr);
}

void	printer::printdate (struct timeval t) {
struct tm tmp;

	if (t. tv_sec + t. tv_usec == 0)
	   return;

	gmtime_r (&(t. tv_sec), &tmp);

	fprintf (stderr, "%02d/%02d/%04d %02d:%02d:%02d",
                 tmp. tm_mday, tmp. tm_mon + 1, tmp. tm_year + 1900,
                 tmp. tm_hour, tmp. tm_min, tmp. tm_sec);
}

void	printer::printtime (struct timeval t) {
struct tm tmp;

	gmtime_r (&(t. tv_sec), &tmp);
        fprintf (stderr, "%02d:%02d:%02d",
	         tmp. tm_hour, tmp. tm_min, tmp. tm_sec);
}


void	printer::cls(void) {
	printf ("\x1b[H\x1b[2J");
}

void	printer::printmonitor (acarsmsg_t * msg, int chn) {
flight_t *fl;

	cls();
	printf ("             Acarsdec monitor "); //printtime(tv);
	printf ("\n Aircraft Flight  Nb Channels     First    DEP   ARR   ETA\n");

	fl	= flight_head;
	while (fl) {
	   int i;
	   printf ("%8s %7s %3d ", fl -> addr, fl -> fid, fl -> nbm);
	   for (i = 0; i < channels; i++)
	      printf ("%c", (fl -> chm & (1 << i)) ?'x':'.');
	   for (;i < 8; i++)
	      printf(" ");
	   printf (" "); printtime (fl -> ts);
	   if (fl -> oooi. sa [0])
	      printf (" %4s ", fl -> oooi. sa);
	   else
	      printf ("      ");
	   if (fl -> oooi. da [0])
	      printf (" %4s ", fl -> oooi. da);
	   else
	      printf ("      ");
	   if (fl -> oooi. eta [0])
	      printf (" %4s ", fl -> oooi. eta);
	   else
	      printf ("      ");
	   printf ("\n");

	   fl = fl -> next;
	}

	fflush(stdout);
}

void printer::printoneline (acarsmsg_t * msg, int chn) {
char txt [30];
char *pstr;

	strncpy (txt, (char *)(msg -> txt), 29);
	txt [29] = 0;
	for (pstr = txt; *pstr != 0; pstr++)
	   if (*pstr == '\n' || *pstr == '\r')
	      *pstr = ' ';

	fprintf (stdout, "#%1d (L:%+3d E:%1d) ", chn, msg -> lvl, msg->err);

	printdate (msg -> messageTime);
	fprintf( stdout, " %7s %6s %1c %2s %4s ",
	                      msg -> addr,
	                      msg -> fid,
	                      msg -> mode,
	                      msg -> label, msg -> no);
	fprintf (stdout, "%s", txt);
	fprintf (stdout, "\n");
	fflush (stdout);
}


void	printer::addFlight (acarsmsg_t * msg, int chn) {
flight_t *fl,*flp;
oooi_t oooi;

	fl	= flight_head;
	flp	= NULL;
	while (fl != NULL) {
	   if (strcmp ((char *)(msg -> addr), (char *)(fl -> addr)) == 0)
	      break;
	   flp	= fl;
	   fl	= fl -> next;
	}

	if (fl == NULL) {
	   fl = (flight_t *)calloc (1, sizeof (flight_t));
	   strncpy (fl -> addr, (char *)(msg -> addr), 8);
	   fl -> nbm	= 0;
	   fl -> ts	= msg -> messageTime;
	   fl -> chm	= 0;
	   fl -> next	= NULL;
	}

	strncpy (fl -> fid, (char *)(msg -> fid), 7);
	fl -> tl	= msg -> messageTime;
	fl -> chm	|= (1 << chn);
	fl -> nbm	+= 1;

	if (DecodeLabel (msg, &oooi)) {
	   if (oooi. da [0])
	      memcpy (fl -> oooi. da, oooi. da, 5);
	   if (oooi. sa [0])
	      memcpy (fl -> oooi. sa, oooi. sa, 5);
	   if (oooi. eta [0])
	      memcpy (fl -> oooi. eta, oooi. eta, 5);
	   if (oooi. gout [0])
	      memcpy (fl -> oooi. gout, oooi. gout, 5);
	   if (oooi. gin [0])
	      memcpy (fl -> oooi. gin, oooi. gin, 5);
	   if (oooi. woff [0])
	      memcpy (fl -> oooi. woff, oooi. woff, 5);
	   if (oooi. won [0])
	      memcpy (fl -> oooi. won, oooi. won, 5);
	}

	if (flp != NULL) {
	   flp	-> next	= fl -> next;
	   fl	-> next	= flight_head;
	}
	flight_head	= fl;
	flp		= NULL;

	while (fl != NULL) {
	   if (fl -> tl. tv_sec < (msg -> messageTime. tv_sec)) {
	      if (flp != NULL) {
	         flp -> next	= fl -> next;
	         free (fl);
	         fl = flp -> next;
	      } else {
	         flight_head = fl -> next;
	         free (fl);
	         fl	= flight_head;
	      }
	   } else {
	      flp	= fl;
	      fl	= fl -> next;
	   }
	}
}


