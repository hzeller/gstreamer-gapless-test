Code to reproduce
 https://bugzilla.gnome.org/show_bug.cgi?id=698306

 This is to investigate various odd behavior of gapless play with URIs.
  - gstreamer-0.1: runs out of threads on gapless playing URIs
  - gstreamer-1.0: does not handle well having the URI set in about-to-finish
                   callback.

To reproduce:

Build both both versions with

     make

(This expects pkg-config to work for gstreamer-0.10 and gstreamer-1.0)

Preparation: provide sound-file via webserver
----------------------------------------------

Run a webserver that serves the test sound file.

Webfsd is a lightweight solution (sudo aptitude install webfs). There
is a file in the sounds/ directory, let's serve that.

     webfsd -r sounds/ -p 9999

Playing URI with gstreamer 0.10
-------------------------------

 Run the binary

     ./test-loop-0.1 http://localhost:9999/test-sound.ogg

In another thread, watch how the number of threads increase over time

     $ while : ; do ps -eLf | grep test-loop | grep -v grep | wc -l ; sleep 1 ; done

.. and after some amount of repeats (~30-100) the whole process just stops
working, with many threads stuck.

Playing URI with gstreamer 1.0
------------------------------

Running the 1.0 binary:

     ./test-loop-1.0 http://localhost:9999/test-sound.ogg

This only plays the URI once and then goes into an endless loop.

Playing a file-uri works fine
-----------------------------

Same thing works fine with both versions, if the input is a file

     ./test-loop-0.1 file://`pwd`/sounds/test-sound.ogg
     ./test-loop-1.0 file://`pwd`/sounds/test-sound.ogg

(works means: plays the same sound indefinetly, and only uses a limited
 amount of threads).

Version
-------
Tested on Ubuntu 12.04/precise. The 0.10 gstreamer version comes natively
with that Ubuntu version, the 1.0.6 version from

    deb http://ppa.launchpad.net/gstreamer-developers/ppa/ubuntu precise main
