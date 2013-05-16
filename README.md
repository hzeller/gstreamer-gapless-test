 Source including Makefile and test sound file on:
 https://github.com/hzeller/gstreamer-gapless-test

 This is to investigate a race condition while doing HTTP streaming that
 prevents "about-to-finish" to work.

 Relevant bugs:
   - [#698750 playbin: gapless playback using "about-to-finish" doesn't work with
      http URIs][gst-bug-698750]
   - [#686153 Gapless using about-to-finish appears broken][gst-bug-686153]

To reproduce:

Build both both versions with

 make

 (This expects pkg-config to work for gstreamer-0.10 and gstreamer-1.0)

Preparation: provide sound-file via webserver
---------------------------------------------

Run a webserver that serves the test sound file.

Webfsd is a lightweight solution (sudo aptitude install webfs). There
is a file in the sounds/ directory, let's serve that.

    webfsd -r sounds/ -p 9999

Playing URI with gstreamer 1.0
------------------------------

### HTTP URI ###

A HTTP URI *does not work with OGG* files:

    ./test-loop-1.0 http://localhost:9999/1234567.ogg

This only plays the URI once and then goes into an endless loop.

As indicated by Bug [#686153][gst-bug-686153], this could be due to not
resetting the buffer position properly. And indeed, if we play multiple
files in a row, with shorter files first, the longer ones pick up about the
same time the first finished; in the following example we hear the full first
file, and the second file somewhere starting in the middle:
 
    ./test-loop-1.0 http://localhost:9999/12.ogg http://localhost:9999/1234567.ogg

This *does work with WAV* files over a HTTP URI

    ./test-loop-1.0 http://localhost:9999/12.wav http://localhost:9999/1234567.wav

### File URI ###

Unlike HTTP, with file URIs, the behavior wrt. *.wav files vs. *.ogg files
is the opposite.

*WAV files do not work*:

    ./test-loop-1.0 file://`pwd`/sounds/12.wav file://`pwd`/sounds/1234567.wav

while *OGG files work*:

    ./test-loop-1.0 file://`pwd`/sounds/12.ogg file://`pwd`/sounds/1234567.ogg

Comparison: this works with gstreamer 0.10
------------------------------------------

All of the examples above do work

    ./test-loop-0.1 http://localhost:9999/12.ogg http://localhost:9999/1234567.ogg
    ./test-loop-0.1 http://localhost:9999/12.wav http://localhost:9999/1234567.wav
    ./test-loop-0.1 file://`pwd`/sounds/12.ogg file://`pwd`/sounds/1234567.ogg
    ./test-loop-0.1 file://`pwd`/sounds/12.wav file://`pwd`/sounds/1234567.wav

Version
-------

Tested on Ubuntu 13.04.
   - gstreamer1.0-plugins-base/ version: 1.0.6-1
   - gstreamer0.10-plugins-base/ version: 0.10.36-1.1ubuntu1



[gst-bug-698750]: https://bugzilla.gnome.org/show_bug.cgi?id=698750
[gst-bug-686153]: https://bugzilla.gnome.org/show_bug.cgi?id=686153
