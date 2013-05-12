Results
-------
The modified test program is supposed to play the given sequence in order,
then repeat the last track until canceled. However, as the listing below
shows, only the first track is played correctly. The second track, when played
for the first time, starts playing at the timestamp where the first track
ended. The second time the second track is supposed to be played, there is no
sound at all. Instead, "about-to-finish" signals are repeated about once a
second, indicating that playback was actually started at the *end* of the
track.

It turns out the issue is similar to bug 686153

	https://bugzilla.gnome.org/show_bug.cgi?id=686153

that shows the same symptons when using a file:// URI with *.wav files (not
tested using this tool).

On the other hand (not shown below), even http:// URIs work fine and don't
show any symptoms of this bug when used with *.mp3 files.

Using a more complex tool (gnome-music) based on playbin and gstreamer-1.0, it
appears like things are even a little more complicated: Using the seek_simple
method of playbin during playback seems to influence the timestamp at which
the next track starts playing. For example, seeking to a position 10 seconds
before the end of the current track, the next track will start playback at
0:10. This seems to be independent of the length of the current track.

These combinations are affected:
* HTTP / ogg
* file / wav  (according to bug 686153)

These combinations are not affected:
* HTTP / mp3
* file / mp3
* file / ogg

```
$ ./test-loop-1.0 http://localhost:9999/1.ogg
Now playing: http://localhost:9999/1.ogg
Running...
[** 1.ogg played correctly **]
about-to-finish    0; setting next to http://localhost:9999/1.ogg
[** No more music and about-to-finish spam: **]
about-to-finish    1; setting next to http://localhost:9999/1.ogg
about-to-finish    2; setting next to http://localhost:9999/1.ogg
[** ... **]
about-to-finish   12; setting next to http://localhost:9999/1.ogg
^C

$ ./test-loop-1.0 http://localhost:9999/1.ogg http://localhost:9999/2.ogg
Now playing: http://localhost:9999/1.ogg
Running...
[** 1.ogg played correctly with duration of 1:55 **]
about-to-finish    0; setting next to http://localhost:9999/2.ogg
[** 2.ogg NOT starting at 0:00, instead at 1:55! **]
about-to-finish    1; setting next to http://localhost:9999/2.ogg
[** No more music and about-to-finish spam: **]
about-to-finish    2; setting next to http://localhost:9999/2.ogg
[** ... **]
about-to-finish   11; setting next to http://localhost:9999/2.ogg
^C

$ ./test-loop-1.0 file://`pwd`/sounds/1.ogg file://`pwd`/sounds/2.ogg
Now playing: file:///home/[censored]/sounds/1.ogg
Running...
[** 1.ogg played correctly with duration of 1:55 **]
about-to-finish    0; setting next to file:///home/[censored]/sounds/2.ogg
[** 2.ogg played correctly with duration of 2:38 **]
about-to-finish    1; setting next to file:///home/[censored]/sounds/2.ogg
[** 2.ogg played correctly with duration of 2:38 **]
^C
```
