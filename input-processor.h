
/*
 *	Copyright (c) 2018
 *	Jan van Katwijk Lazy Chair Computing
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
 *
 */
#ifndef	__INPUT_PROCESSOR__
#define	__INPUT_PROCESSOR__

#include	"acars-constants.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"channel-handler.h"
#include        <stdio.h>
#include        <stdint.h>
#include        <time.h>
#include        <stdio.h>
#include        <thread>
#include        <mutex>
#include        "semaphore.h"
#include        <atomic>
#include	<vector>

class	inputProcessor {
public:
		inputProcessor (deviceHandler *,
	                        std::vector<channelHandler *>*);
		~inputProcessor (void);
	void	start		(void);
	void	stop		(void);

private:
	void		run		(void);
	deviceHandler	*theDevice;
	int		outrate;
	std::vector<channelHandler *> *handlers;
	std::atomic<bool>	running;
	std::thread		threadHandle;
};
#endif
	
