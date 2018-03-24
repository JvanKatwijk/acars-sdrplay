# acars-sdrplay

acars is a partial rewrite in C++ of
[acarsdec][https://github.com/TLeconte/acarsdec], written by Thierry Leconte.
The reason for this rewrite was the idea of including the SDRplay device
as input device. 

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
------------------------------------------------------------------------


# Copyright

Copyright Jan van Katwijk <J.vanKatwijk@gmail.com>.
Lazy Chair Computing

This software is part of the Qt-DAB, Qt-DAB is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version, taking into account the licensing conditions of the parts of the software that are derived from wotk of others.

This software uses parts of dabtools. Excerpt from the README of dabtools reads

"dabtools is written by Dave Chapman <dave@dchapman.com>
   
Parts of the code in eti-backend are copied verbatim (or with trivial modifications) from David Crawley's OpenDAB and hence retain his copyright."

Obviously, the copyrights for the parts copied (or directly derived) from the dabtools remain with Dave Chapman.
