
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
 *	The channel handler code is based on the code
 *	in the air.c and rtl.c files of acars of Thierry Leconte.
 *	All rights gratefully acknowledged.
 */
#ifndef	__CHANNEL_HANDLER__
#define	__CHANNEL_HANDLER__
#include	<stdio.h>
#include	<stdint.h>
#include	"acars-constants.h"
#include	"channel-decoder.h"
#include	"fir-filters.h"
#include	"decimating_filter.h"
#include	"printer.h"

class	channelHandler {
public:
	channelHandler	(printer *,int, int, int, bool, std::complex<float> *);
	~channelHandler	(void);
void	processData	(std::complex<float> *, int);
private:
	int		deviceRate;
	channelDecoder	theDecoder;
	int		offset;
	int		channelno;
	bool		verbose;
	std::complex<float> *oscillatorTable;
	int		Phase;
	int		Ratio;
//	decimating_filter	theFilter;
	decimatingFIR	theFilter;
	float		outVector [512];
	int		outPointer;
};

#endif

