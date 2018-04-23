# acars-sdrplay

acars is a partial rewrite in C++ of
[acarsdec][https://github.com/TLeconte/acarsdec], written by Thierry Leconte.
The reason for this rewrite was the idea of including the SDRplay device
as input device which required some reorganization of the decoding.

------------------------------------------------------------------------

	This is work in progress

------------------------------------------------------------------------

All copyrights of Thierry Leconte for the algorithmic part and
the interpretation of the incoming data is gratefully acknowledged.

------------------------------------------------------------------------
------------------------------------------------------------------------

**Creating an executable under Linux**

The source directory contains a file "CMakeLists.txt", the normal
way of building an executable is 

        mkdir build
        cd build
        cmake .. -DXXX=ON
        make
        sudo make install

where XXX is one of the supported input devices, i.e. SDRPLAY, AIRSPY,
or RTLSDR. Note that it is quite well possible to have more than
one device included. In runtime, the software will detect which
device is connected and open it.

------------------------------------------------------------------------

**Commandline parameters**

------------------------------------------------------------------------

The program is commandline driven, parameters are:
* -f add a frequency (specified in KHz);
* -g set the gain at the connected device (scale 1 .. 100);
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

-------------------------------------------------------------------------
-------------------------------------------------------------------------

# Copyright

Copyright Jan van Katwijk <J.vanKatwijk@gmail.com>.
Lazy Chair Computing

This software is part of the sdr-j suite. The sdr-j suite is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version, taking into account the licensing conditions of the parts of the software that are derived from work of others.
