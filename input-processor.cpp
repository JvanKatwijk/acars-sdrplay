#
/*
 *	Copyright (c) 2018
 *	Jan van Katwijk, lazy Chair Computing
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
 */

//
//	The "inputProcessor" is the link between the
//	connected device and the channel handler. It reads
//	data, and passes it on to each of the channelhandlers.
//
#include	"input-processor.h"
#include	<unistd.h>

	inputProcessor::inputProcessor (deviceHandler *theDevice,
	                                std::vector<channelHandler *>*h) {
	this	-> theDevice	= theDevice;
	this	-> handlers	= h;
}

	inputProcessor::~inputProcessor (void) {
	if (running. load ()) {
	   running. store (false);
	   threadHandle. join ();
        }
}

void	inputProcessor::start  (void) {
	running. store (true);
	threadHandle = std::thread (&inputProcessor::run, this);
}

void	inputProcessor::stop   (void) {
        if (running. load ()) {
           running. store (false);
           threadHandle. join ();
        }
}

void	inputProcessor::run (void) {
	std::complex<float> inbuf [4 * 5120];
	theDevice	-> restartReader ();
	running. store (true);
	while (running. load ()) {
	   while (running. load () && (theDevice -> Samples () < 4 * 5120))
	      usleep (1000);
	   theDevice	-> getSamples (inbuf, 4 * 5120);
	   for (uint16_t i = 0; i < handlers -> size (); i ++) {
	      channelHandler *theHandler = handlers -> at (i);
	      theHandler -> processData (inbuf, 4 * 5120);
	   }
	}
}

