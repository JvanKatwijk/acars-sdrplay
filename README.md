# acars-sdrplay

acars is a (partial) rewrite in C++ of
[acarsdec][https://github.com/TLeconte/acarsdec], written by Thierry Leconte.
The reason for this rewrite was including the SDRplay device
as input device which required some reorganization of the decoding.

------------------------------------------------------------------------

------------------------------------------------------------------------

Copyright of Thierry Leconte for the algorithmic part and
the interpretation of the incoming data is gratefully acknowledged.

------------------------------------------------------------------------
Supported devices
------------------------------------------------------------------------

Since I an using an SDRplay device, and I wanted to use the - excellent -
acarsdec software, I decided to add support for the SDRplay. By doing that
the structure of the program was changed, it was rewritten in C++.

It supports, next to the SDRplay, RTLSDR based dabsticks, the AIRspy
devices and HACKRF devices.
One modification - following from the way the rewrite was implemented -
is that the number of frequencies that can be listened to is - in
principle - unlimited.  

Support for other devices is easy to implement, a pretty straigh forward
interface between device support and the processing chain is defined.
Not implemented is support for file input, not for soundcard input (my
laptop does not have a microphone or line input). 

------------------------------------------------------------------------
**Creating an executable under Linux**
------------------------------------------------------------------------

The source directory contains a file "CMakeLists.txt", the normal
way of building an executable is 

        mkdir build
        cd build
        cmake .. -DXXX=ON
        make
        sudo make install

where XXX is any of the supported input devices, i.e. SDRPLAY, AIRSPY,
RTLSDR or HACKRF.

Note that it is quite well possible to include  more than
one device in the configuration. In runtime, the software will detect which
device is connected and (try to) open it.

------------------------------------------------------------------------
**Commandline parameters**
------------------------------------------------------------------------


The program is commandline driven, parameters are:
* -f add a frequency (specified in KHz);
* -a set the autogain at the selected device;
* -p set the ppm correction at the selected device;
* -v set the verbose option, will generate more output
* -o set the output: 
*	OUTTYPE_NONE 0, no output
*	OUTTYPE_ONELINE 1, brief output
*	OUTTYPE_STD 2, normal output
*	OUTTYPE_MONITOR 3, monitor traffic
*	OUTTYPE_JSON 4, output in JSON format
* -N identaddr will send the output also to the specified internet addres
     (if connection can be made to someone who is listening)
* -n identaddr will send the output also to the specified internet address,
     assuming that a PLANEPLOTTER is listening.

Device specific parameters are the gain settings:

For the RTLSDR device

* -g N does not have a meaning
* -s N set  the gain, in tenths of dB's, supported values depend on the device

For the AIRspy device

* -g N does not have a meaning
* -s N sets the gain on a scale 0 .. 100

For the HACKRF device

* -g N sets the lna gain (range 0 .. 40)
* -s N sets the vga gain (range 0 .. 62)

For the SDRplay device

* -g N sets the lnaState. The range of values depends on the device
* -s N sets the GAIN REDUCTION (range 20 .. 59)

e.g. for the SDRplay I use
	
	acars-0.5 -g 2 -s 25 -f 131525 -f 131725

   
-------------------------------------------------------------------------
-------------------------------------------------------------------------

# Copyright 2018

Copyright Jan van Katwijk <J.vanKatwijk@gmail.com>.
Lazy Chair Computing

This software is part of the sdr-j suite. The sdr-j suite is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version, taking into account the licensing conditions of the parts of the software that are derived from work of others.
