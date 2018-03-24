#
/*
 *    Copyright (C) 2018
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the acars
 *
 *    acars is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    acars is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with acars; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	__SEMAPHORE__
#define	__SEMAPHORE__

using namespace std;

#include	<thread>
#include	<mutex>
#include	<condition_variable>

class	Semaphore {
private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
public:
	Semaphore (int count_ = 0) : count {count_} {}

void	Release (void) {
	std::unique_lock<mutex>lck (mtx);
	++count;
	cv. notify_one ();
}

void	acquire (void) {
	unique_lock <std::mutex> lck (mtx);
	while (count == 0) {
	   cv. wait (lck);
	}
	-- count;
}

bool	tryAcquire (int delay) {
	unique_lock <std::mutex> lck (mtx);
	if (count == 0) {
	   auto now = std::chrono::system_clock::now ();
           cv. wait_until (lck, now + std::chrono::milliseconds (delay));
	}
	if (count == 0)
	   return false;
	-- count;
	return true;
}
};
#endif
