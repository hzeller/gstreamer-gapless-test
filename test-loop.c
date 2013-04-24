/*
  Source including Makefile and test sound file on:
  https://github.com/hzeller/gstreamer-gapless-test

  This is to investigate various odd behavior of gapless play with URIs with
  different versions of gstreamer. The relevant bugs are:

  - gstreamer-0.1: "playbin2" leaks threads playing gapless from network URIs
       https://bugzilla.gnome.org/show_bug.cgi?id=698750

  - gstreamer-1.0: Gapless playing using 'about-to-finish' callback fails
                   with HTTP-URIs
        https://bugzilla.gnome.org/show_bug.cgi?id=698306

 To reproduce:

 Build both both versions with

 make

 (This expects pkg-config to work for gstreamer-0.10 and gstreamer-1.0)

 == Preparation: provide sound-file via webserver ==
 Run a webserver that serves the test sound file.

 Webfsd is a lightweight solution (sudo aptitude install webfs). There
 is a file in the sounds/ directory, let's serve that.

 webfsd -r sounds/ -p 9999

 == Playing URI with gstreamer 0.10 ==
 Run the binary
 ./test-loop-0.1 http://localhost:9999/test-sound.ogg

 In another shell, watch how the number of threads increase over time

 $ while : ; do ps -eLf | grep test-loop | grep -v grep | wc -l ; sleep 1 ; done

 .. and after some amount of repeats (~30-100) the whole process just stops
 working, with many threads stuck.

 == Playing URI with gstreamer 1.0 ==
 ./test-loop-1.0 http://localhost:9999/test-sound.ogg

 This only plays the URI once and then goes into an endless loop.

 == Playing a file-uri ==
 Same thing works fine with both versions, if the input is a file

 ./test-loop-0.1 file://`pwd`/sounds/test-sound.ogg
 ./test-loop-1.0 file://`pwd`/sounds/test-sound.ogg

 (works means: plays the same sound indefinetly, and only uses a limited
  amount of threads).
 */

#include <assert.h>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>

struct NextStreamData {
	GstElement *pipeline;
	int count;
	const char *next_uri;
};

static void prepare_next_stream(GstElement *obj, gpointer userdata) {
	struct NextStreamData *data = (struct NextStreamData*) userdata;
	assert(data->next_uri);
	g_print("about-to-finish %4d; setting next to %s\n",
		data->count, data->next_uri);
	g_object_set(G_OBJECT(data->pipeline), "uri", data->next_uri, NULL);
	data->count++;
}

int main (int argc, char *argv[]) {
	GMainLoop *loop;
	GstElement *pipeline;

	/* Initialisation */
	gst_init (&argc, &argv);

	loop = g_main_loop_new (NULL, FALSE);

	/* Check input arguments */
	if (argc != 2) {
		g_printerr ("Usage: %s <filename or URL>\n", argv[0]);
		return -1;
	}

	/* Create playbin. Different names in different versions  */
#if (GST_VERSION_MAJOR < 1)
	pipeline = gst_element_factory_make("playbin2", "play");
#else
	pipeline = gst_element_factory_make("playbin", "play");
#endif

	/* Register about-to-finish callback to re-set the URI */
	struct NextStreamData replay_data;
	replay_data.pipeline = pipeline;
	replay_data.count = 0;
	replay_data.next_uri = argv[1];   // let's loop forever the same URI
	g_signal_connect(pipeline, "about-to-finish",
		G_CALLBACK(prepare_next_stream), &replay_data);

	/* Set initial URI */
	g_object_set (G_OBJECT (pipeline), "uri", argv[1], NULL);

	g_print ("Now playing: %s\n", argv[1]);
	gst_element_set_state (pipeline, GST_STATE_PLAYING);

	/* Iterate */
	g_print ("Running...\n");
	g_main_loop_run (loop);

	/* since we loop endlessly, the follwing is never reached */

	/* Out of the main loop, clean up nicely */
	g_print ("Returned, stopping playback\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);

	g_print ("Deleting pipeline\n");
	gst_object_unref (GST_OBJECT (pipeline));
	g_main_loop_unref (loop);

	return 0;
}
