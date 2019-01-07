#
/*
 *	Copyright (c) 2018
 *	Jan van Katwijk, Lazy Chair Computing
 *
 *   This code is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 */

#include        <unistd.h>
#include        <signal.h>
#include        <getopt.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<stdint.h>
#include	<unistd.h>
#include	<string.h>
#include	<math.h>
#include	<signal.h>
#include	<atomic>
#include	<complex>
#include	"acars-constants.h"
#include	"acars.h"
#include	<vector>

static
std::atomic<bool>	running;
void    printOptions    (void);

static void sighandler (int signum) {
        fprintf (stderr, "Signal caught, terminating!\n");
	running. store (false);
}

int	main (int argc, char ** argv) {
int c;
struct sigaction sigact;
std::vector<int> freqList;
int	lnaGain		= 0;
int	vgaGain		= 0;
bool	autogain	= false;
int	outtype		= 0;
bool	verbose		= false;
int	ppmCorrection	= 0;
char	*rawAddr	= NULL;

	while ((c = getopt (argc, argv, "f:g:s:al:o:p:vhn:N:")) != EOF) {
	int help;
	   switch (c) {
	      case 'f':
	         freqList. push_back (atoi (optarg) * 1000);
	         break;
	      case 'g':
	         lnaGain		= atoi (optarg);
	         break;
	      case 's':
	         vgaGain		= atoi (optarg);
	         break;
	      case 'a':
	         autogain	= true;
	         break;
	      case 'o':
	         help		= atoi (optarg);
	         if ((outtype & ANY_OUTPUT) == 0) {
	            if (help == 1) 
	              outtype |= 1;
	            else
	            if (help == 2)
	              outtype |= 2;
	            else
	            if (help == 3)
	              outtype |= 4;
	            else
	            if (help == 4)
	              outtype |= 010;
	         }
	         break;

	      case 'p':
	         ppmCorrection	= atoi (optarg);
	         break;

	      case 'v':
	         verbose	= true;
	         break;

	      case 'n':
	         if ((outtype & ANY_NETOUT) == 0) {
	            rawAddr	= optarg;
	            outtype	|= NETLOG_PLANEPLOTTER;
	         }
	         break;

	      case 'N':
	         if ((outtype & ANY_NETOUT) == 0) {
	            rawAddr	= optarg;
	            outtype	|= NETLOG_NATIVE;
	         }
	         break;

	      case 'J':
	         if ((outtype & ANY_NETOUT) == 0) {
	            rawAddr	= optarg;
	            outtype	|= NETLOG_JSON;
	         }
	         break;

	      case 'h':
	         printOptions ();
	         exit (1);

	      default:
	         break;
	   }
	}

	if ((outtype & ANY_OUTPUT) == 0)
	   outtype |= OUTTYPE_STD;

	sigact.sa_handler = sighandler;
	sigemptyset (&sigact.sa_mask);
	sigact.sa_flags = 0;

	if (freqList. size () == 0)	// choose a default
	   freqList. push_back (131725 * 1000);

	fprintf (stderr, "rawAddr = %s\n", rawAddr);
        acars *my_acars = new acars (&freqList, lnaGain, vgaGain, ppmCorrection,
	                              verbose, autogain, outtype, rawAddr);
	running. store (true);
	while (running. load ()) 
	   sleep (1);
	delete my_acars;
}

void	printOptions	(void) {
	fprintf (stderr, "acars options: \n"
                         "-f  N: add frequency N to the list of frequencies \n"
	                 "-g  N: hackrf device: set lna gain\n"
	                 "-L  N: sdrplay device: set lna state\n"
	                 "-G  N: sdrplay device: set Gain reduction in dB\n"
	                 "-s  N: hackrf device: set vgaGain\n"
	                 "-s  N: sdrplay device: set Gain reduction in dB\n"
	                 "-g  N: rtlsdr device: set Gain tenths of dB\n"
	                 "-a   : set the autogain of the device\n"
	                 "-p  N: set the ppm correction \n"
	                 "-v   : set verbose option\n"
	                 "-o  N: set output type (0, 1, 2, 3)\n"
	                 "-n  inetaddr : dump output to inetaddr\n"
	                 "-N  inetaddr : dump output to inetaddr\n"
	                 "-J  inetaddr : dump output to inetaddr\n");
}


	
