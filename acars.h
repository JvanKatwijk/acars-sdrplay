#
/*
 *    Copyright (C)  2015 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#ifndef __ACARS__
#define __ACARS__

#include	"ringbuffer.h"
#include	<vector>
#include	"printer.h"

class	deviceHandler;
class	spectrumhandler;
class	channelHandler;

#include	"channel-decoder.h"
#include	"input-processor.h"

/*
 *	Driver class.
 */
class acars {
public:
		acars	(std::vector<int> *,
	                 int, int, int, bool, bool, int, char *);
		~acars	(void);

private:
	deviceHandler		*theDevice;
	inputProcessor		*theProcessor;
	int			ppmCorrection;
	printer			thePrinter;
	int32_t			sampleCount;
	std::vector<channelHandler *> channels;
	int			findFreq	(std::vector<int> *);
	std::complex<float>	*oscillatorTable;
	bool			verbose;
};

#endif

