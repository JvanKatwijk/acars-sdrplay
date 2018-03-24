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
 *	Jan van Katwijk, lazy Chair Computing
 *	This file is (very) loosely based on the input handling
 *	of the original acars software from Thierry Leconte
 */
#include	"channel-handler.h"


	channelHandler::channelHandler (printer *thePrinter,
	                                int	inputRate,
	                                int	offset,
	                                int	channelno, 
	                                bool	verbose,
	                                std::complex<float> *table):
	                                theDecoder (thePrinter,
	                                            channelno, verbose),
	                                theFilter (16 * inputRate / WORKING_RATE + 1,
	                                           5000,
	                                           inputRate, 
	                                           inputRate / WORKING_RATE) {
	fprintf (stderr, "handler %d has offset %d\n", channelno, offset);
	this	-> deviceRate	= inputRate;
	this	-> offset	= offset;
	this	-> channelno	= channelno;
	this	-> verbose	= verbose;
	oscillatorTable		= table;
	Phase	= 0;
	outPointer	= 0;
}

	channelHandler::~channelHandler (void) {
}

void	channelHandler::processData (std::complex<float> *input, int length) {
int	i;
std::complex<float> o;

	for (i = 0; i < length; i ++) {
	   std::complex<float>temp = input [i] * oscillatorTable [Phase];
	   Phase -= offset;
	   if (Phase < 0)
	      Phase += deviceRate;
	   else
	   if (Phase >= deviceRate)
	      Phase -= deviceRate;
	   if (theFilter. Pass (temp, &o)) {
	      outVector [outPointer ++] = abs (o);
	      if (outPointer >= 512) {
	         theDecoder. demodMSK (outVector, 512);
	         outPointer = 0;
	      }
	   }
	}
}

