svmidi - simple virtual midi keyboard
=====================================
This is very simple virtual midi keyboard. It handle key presses and send 
midi messages to the synth. It also handle instrument and octave selection.

*looking for a better name*

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

Authors
-------
On LICENSE and on manpage
