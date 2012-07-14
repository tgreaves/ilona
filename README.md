ilona
=====

A simple IRC bot.

This was originally written for the Amiga in 1997.  The various sources for that may be found in the 'historic' directory:

* Ilona: As released on Aminet at the time.
* Ilona_KeyMaker: Key generator for Ilona.  Some outstanding algorithms at play here (!)
* Ilona_KeyReader: Reads generated keys.
* Ilona2: A partial re-write of Ilona in C++.  The intention was to allow multiple bots from within a single executable (OO).
* ilona3: Goal the same as Ilona2, although written in C.  

I have now been working on getting this to run nicely under Linux/gcc.  This is the source in 'src', based on that in 'ilona3'.  The Amiga support has been dropped from this.

For testing, ngircd is a good IRC server: http://ngircd.barton.de/

Compiling
---------

* make

Job done.

Amiga Compiling
---------------

If you want to try compiling the 'historic' sources, the StormC environment was used.

