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
 *	Jan van Katwijk, Lazy Chair Computing
 *	This file is the driver program for the acars software
 */
#include        <unistd.h>
#include        <vector>
#include	"acars-constants.h"
#include	"acars.h"
#include	"channel-handler.h"
#include	"fir-filters.h"
#include	"printer.h"
#ifdef	HAVE_SDRPLAY
#include	"sdrplay-handler.h"
#endif
#ifdef	HAVE_AIRSPY
#include	"airspy-handler.h"
#endif
#ifdef	HAVE_RTLSDR
#include	"rtlsdr-handler.h"
#endif
#ifdef __MINGW32__
#include	<iostream>
#include	<windows.h>
#endif
//
/*
 *	We use the creation function merely to set up the
 *	user interface and make the connections between the
 *	gui elements and the handling agents. All real action
 *	is embedded in actions, initiated by gui buttons
 */

	acars::acars (std::vector<int> *freqList,
	              int	gain,
	              int	ppmCorrection,
	              bool	verbose,
	              bool	autogain,
	              int	outtype):
	                  thePrinter (20, outtype, freqList -> size () ){
int	i;
int	frequency	= findFreq (freqList);

	if (frequency == -1)
	   exit (22);
	this	-> verbose	= verbose;
//	before printing anything, we set
	setlocale (LC_ALL, "");
#ifdef	HAVE_SDRPLAY
	try {
	   theDevice	= new sdrplayHandler (2400000,
	                                      frequency,
	                                      ppmCorrection,
	                                      gain,
	                                      autogain,
	                                      0, 0);

	} catch (int e) {
	}
#endif
#ifdef	HAVE_AIRSPY
	try {
	   theDevice	= new airspyHandler (2400000,
	                                     frequency,
	                                     0,
	                                     gain);
	} catch (int e) {
	}
#endif
#ifdef	HAVE_RTLSDR
	try {
	   theDevice	= new rtlsdrHandler (2400000,
	                                     frequency,
	                                     0,
	                                     gain,
	                                     autogain,
	                                     0);
	} catch (int e) {
	}
#endif

	if (theDevice == NULL) {
	   fprintf (stderr, "we give up\n");
	   exit (1);
	}

	int deviceRate	= theDevice	-> getRate ();
	oscillatorTable	= new std::complex<float> [deviceRate];
	for (i = 0; i < deviceRate; i ++)
	   oscillatorTable [i] = std::complex<float> (
	                           cos ((float)i / deviceRate * 2 * M_PI),
	                           sin ((float)i / deviceRate * 2 * M_PI));
	int finalFreq	= findFreq (freqList);
	for (i = 0; i < freqList -> size (); i ++) {
	   channelHandler *t = new channelHandler (&thePrinter,
	                                           deviceRate,
	                                           freqList -> at (i) - finalFreq,
	                                           i,
	                                           verbose,
	                                           oscillatorTable);
	   channels. push_back (t);
	}
	   
	theProcessor 	= new inputProcessor (theDevice, &channels);
	theProcessor	-> start ();
	thePrinter. start ();
}

	acars::~acars (void) {
	theProcessor	-> stop ();
	thePrinter. stop ();
	delete theProcessor;
	for (uint16_t i = 0; i < channels. size (); i ++)
	   delete channels. at (i);
}

int	acars::findFreq	(std::vector<int> *freqList) {
int i;
int	maxFreq	= -1;
int	minFreq	= 2000000000;
	for (i = 0; i < freqList -> size (); i ++) {
	   fprintf (stderr, "frequency %d is %d\n", i, freqList -> at (i));
	   if (freqList -> at (i) > maxFreq)
	      maxFreq = freqList -> at (i);
	   if (freqList -> at (i) < minFreq)
	      minFreq = freqList -> at (i);
	}
	if (maxFreq > minFreq + 2000000)
	   return -1;
	return minFreq + (maxFreq - minFreq) / 2;
}

