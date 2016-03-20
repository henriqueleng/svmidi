svmidi - simple virtual midi keyboard
=====================================
This is very simple virtual midi keyboard. It handle key presses and send 
midi messages to the synth. It also handle instrument and octave selection.
Yes, this program on send midi messages.

*looking for a better name*

Requirements
------------
This program is based on Xlib, so you will need Xlib header files.

Since OpenBSD is my main OS, and I don't have any Linux machine neither
experience with ALSA, this program currently only works with sndio, so you
may try it by installing sndio on your system, or sending me a patch with 
ALSA support. Read TODO if you intend to do so.

Installation
------------

    make clean install

Running
-------

Read manpage and try the -h option.

Authors
-------
On LICENSE and on manpage
