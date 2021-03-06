/*
 * Illustrating bugs
 *  https://bugzilla.gnome.org/show_bug.cgi?id=698750
 *  https://bugzilla.gnome.org/show_bug.cgi?id=686153
 *
 * For details how to run, see
 *   https://github.com/hzeller/gstreamer-gapless-test/blob/master/README.md
 */

#include <assert.h>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>

struct NextStreamData {
	GstElement *pipeline;
	int count;
	int uri_count;
	char **uris;
};

static void prepare_next_stream(GstElement *obj, gpointer userdata) {
	struct NextStreamData *data = (struct NextStreamData*) userdata;
	const char* next_uri = data->uris[(data->count+1) % data->uri_count];
	g_print("about-to-finish %4d; setting next to %s\n",
		data->count, next_uri);
	g_object_set(G_OBJECT(data->pipeline), "uri", next_uri, NULL);
	data->count++;
}

int main (int argc, char *argv[]) {
	GMainLoop *loop;
	GstElement *pipeline;

	/* Initialisation */
	gst_init (&argc, &argv);

	loop = g_main_loop_new (NULL, FALSE);

	/* Check input arguments */
	if (argc < 2) {
		g_printerr ("Usage: %s <list of space-delimited filenames or URLs>\n",
			argv[0]);
		return -1;
	}

	/* Create playbin. Different names in different versions  */
#if (GST_VERSION_MAJOR < 1)
	pipeline = gst_element_factory_make("playbin2", "play");
#else
	pipeline = gst_element_factory_make("playbin", "play");
#endif
	assert(pipeline != NULL);

	/* Register about-to-finish callback to re-set the URI */
	struct NextStreamData replay_data;
	replay_data.pipeline = pipeline;
	replay_data.count = 0;
	replay_data.uris = &(argv[1]);
	replay_data.uri_count = argc - 1;
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
