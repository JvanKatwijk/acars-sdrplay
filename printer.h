
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
 *	The printer code is largely based on the output.c file
 *	of acars of Thierry Leconte. Added is a simple spool
 *	mechanism.
 *	
 */
#ifndef	__ACARS_PRINTER__
#define	__ACARS_PRINTER__

#include	<stdio.h>
#include	<stdint.h>
#include	<time.h>
#include        <stdio.h>
#include        <thread>
#include        <mutex>
#include	"semaphore.h"
#include        <atomic>

#define OUTTYPE_NONE 0
#define OUTTYPE_ONELINE 1
#define OUTTYPE_STD 2
#define OUTTYPE_MONITOR 3
#define OUTTYPE_JSON 4

typedef struct {
        unsigned char mode;
        unsigned char addr[8];
        unsigned char ack;
        unsigned char label[3];
        unsigned char bid;
        unsigned char no[5];
        unsigned char fid[7];
        unsigned char bs, be;
        unsigned char txt[250];
        int err, lvl;
	int	channel;
	struct timeval	messageTime;
} acarsmsg_t;

typedef struct {
        char da[5];
        char sa[5];
        char eta[5];
        char gout[5];
        char gin[5];
        char woff[5];
        char won[5];
} oooi_t;

typedef struct flight_s flight_t;
struct flight_s {
        flight_t	*next;
        char		addr [8];
        char		fid [7];
        struct timeval ts, tl;
        int		chm;
        int		nbm;
        oooi_t 		oooi;
};


class	printer {
public:
		printer		(int, int, int);
		~printer	(void);
	void	output_msg	(int, uint8_t *, int16_t, struct timeval);
	void	start		(void);
	void	stop		(void);
private:
	void	run		(void);
	void    process_msg	(int, uint8_t *, int16_t, struct timeval);
	void	printmsg	(acarsmsg_t *);
	void	printdate	(struct timeval t);
	void	printtime	(struct timeval t);
	void    cls		(void);
	void	printmonitor	(acarsmsg_t * msg, int chn);
	void	printoneline	(acarsmsg_t * msg, int chn);
	void	addFlight	(acarsmsg_t * msg, int chn);
	int	outtype;
	int	channels;
	int	slots;
	std::atomic<bool>       running;
        std::thread		threadHandle;
	Semaphore       freeSlots;
        Semaphore       usedSlots;
	int		numberofSlots;
        int16_t         nextIn;
        int16_t         nextOut;

	struct {
	   int	channel;
	   uint8_t	blk_txt [255];
	   int	blk_len;
	   struct timeval blk_tm;
	} theData [20];
};

#endif
