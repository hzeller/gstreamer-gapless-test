  Source including Makefile and test sound file on:
  https://github.com/hzeller/gstreamer-gapless-test

  This is to investigate a race condition while doing HTTP streaming that
  prevents "about-to-finish" to work.

  Relevant bug:
   playbin: gapless playback using "about-to-finish" doesn't work with http URIs
   https://bugzilla.gnome.org/show_bug.cgi?id=698750

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

 ./test-loop-1.0 http://localhost:9999/test-sound.ogg

 This only plays the URI once and then goes into an endless loop.

Comparison: this works with gstreamer 0.10
------------------------------------------
 Run the binary
 ./test-loop-0.1 http://localhost:9999/test-sound.ogg

Playing a file-uri
------------------
 Same thing works fine with both versions, if the input is a file

 ./test-loop-0.1 file://`pwd`/sounds/test-sound.ogg
 ./test-loop-1.0 file://`pwd`/sounds/test-sound.ogg

Version
-------
Tested on Ubuntu 12.04/precise. The 0.10 gstreamer version comes natively
with that Ubuntu version, the 1.0.6 version from

    deb http://ppa.launchpad.net/gstreamer-developers/ppa/ubuntu precise main
