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
 *
 *
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
#include	"acars.h"
#include	<vector>

static
std::atomic<bool>	running;

static void sighandler (int signum) {
        fprintf (stderr, "Signal caught, terminating!\n");
	running. store (false);
}

int	main (int argc, char ** argv) {
int c;
struct sigaction sigact;
std::vector<int> freqList;
int	frequency	= 131725;
int	gain		= 90;
bool	autogain	= false;
int	outtype	= 2;
bool	verbose	= false;
int	ppmCorrection	= 0;

	while ((c = getopt (argc, argv, "f:g:al:o:p:v")) != EOF) {
	   switch (c) {
	      case 'f':
	         freqList. push_back (atoi (optarg) * 1000);
	         break;
	      case 'g':
	         gain		= atoi (optarg);
	         break;
	      case 'a':
	         autogain	= true;
	         break;
	      case 'o':
	         outtype	= atoi (optarg);
	         break;
	      case 'p':
	         ppmCorrection	= atoi (optarg);
	         break;
	      case 'v':
	         verbose	= true;
	         break;
	      default:
	         break;
	   }
	}

	sigact.sa_handler = sighandler;
	sigemptyset (&sigact.sa_mask);
	sigact.sa_flags = 0;

	if (freqList. size () == 0)	// choose a default
	   freqList. push_back (131725 * 1000);

        acars *my_acars = new acars (&freqList, gain, ppmCorrection,
	                              verbose, autogain, outtype);
	running. store (true);
	while (running. load ()) 
	   sleep (1);
	delete my_acars;
}

