svmidi - simple virtual midi keyboard
=====================================
This is very simple virtual midi keyboard. It handle key presses and send 
midi messages to the synth. It also handle instrument and octave selection.

Requirements
------------
This program is based on Xlib, so you will need Xlib header files.
You will also need header files for ALSA or sndio.

Screenshot
----------
![svmidi screenshot](http://henriqueleng.github.io/media/svmidi.png)

Installation
------------

    make clean install

Running
-------

Read manpage and try the -h option.

Example
-------

If you are on Linux:

- Install fluidsynth
- Get a free soundfont on the internet, like [Arachno](http://www.arachnosoft.com/main/download.php?id=soundfont), 
a really good one with a lot of intruments.
- Launch fluidsynth, with -a alsa if you are on linux:

    fluidsynth -a alsa media/sf2/soundfont.sf2

- Launch svmidi

    svmidi -o 2

- Verify the clients' numbers with

    cat /proc/asound/seq/clients

- Connect svmidi to fluidsynth

    aconnect 129:0 128:0

Have Fun!

TODO
----

The main task now is to make the code as elegant and organized as possible,
therefore also making the tool safe and well behaved.

Contributing
------------
Send patches to my email, or do it via github.

Authors
-------
On LICENSE and on manpage
