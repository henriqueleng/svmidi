![svmidi screenshot](http://henriqueleng.github.io/media/svmidi.png)

svmidi - simple virtual midi keyboard
=====================================
This is very simple virtual midi keyboard. It handle key presses and send 
midi messages to the synth. It also handle instrument and octave selection.

*looking for a better name*

Requirements
------------
This program is based on Xlib, so you will need Xlib header files.

Since OpenBSD is my main OS, and I don't have any Linux machine neither
experience with ALSA, this program currently only works with sndio, so you
may try it by installing sndio on your system, or sending me a patch with 
ALSA support. Read bellow if you intend to do so.

Installation
------------

    make clean install

Running
-------

Read manpage and try the -h option.

Example
-------

- Install fluidsynth
- Get a free soundfont on the internet, like [Arachno](http://www.arachnosoft.com/main/download.php?id=soundfont), 
a really good one with a lot of intruments.
- Launch fluidsynth
    fluidsynth media/sf2/soundfont.sf2
- Launch svmidi and have fun
	svmidi -o 2

Contributing
------------
Send patches to my email, or do it via github.

If you intendo to implement ALSA support, make sure to do so in a two files
alsa.c alsa.h, they must have the same functions with the same name 
as in sndio.c, sndio.h, so there will be no need to fill the code with lots 
of #ifdef's. Makefile already have a line to that, and svmidi.c too.

Authors
-------
On LICENSE and on manpage
