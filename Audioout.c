/* GStreamer
 * Copyright (C) 2006 Stefan Kost <ensonic@users.sf.net>
 * Copyright (C) 2008 Jan Schmidt <jan.schmidt@sun.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <gst/gst.h>

static guint spect_bands = 100;

static guint AUDIOFREQ = 32000;

/* receive spectral data from element message */
static gboolean
message_handler (GstBus * bus, GstMessage * message, gpointer data)
{
  if (message->type == GST_MESSAGE_ELEMENT) {
    const GstStructure *s = gst_message_get_structure (message);
    const gchar *name = gst_structure_get_name (s);
    GstClockTime endtime;

    if (strcmp (name, "spectrum") == 0) {
      const GValue *magnitudes;
      const GValue *phases;
      const GValue *mag, *phase;
      gdouble freq;
      guint i;

      if (!gst_structure_get_clock_time (s, "endtime", &endtime))
        endtime = GST_CLOCK_TIME_NONE;

    //  g_print ("New spectrum message, endtime %" GST_TIME_FORMAT "\n",
     //     GST_TIME_ARGS (endtime));

      magnitudes = gst_structure_get_value (s, "magnitude");
      phases = gst_structure_get_value (s, "phase");

      for (i = 0; i < spect_bands; ++i) {
        freq = (gdouble) ((AUDIOFREQ / 2) * i + AUDIOFREQ / 4) / spect_bands;
        mag = gst_value_list_get_value (magnitudes, i);
        phase = gst_value_list_get_value (phases, i);

        if (mag != NULL && phase != NULL) {
   //       g_print ("%g %f\n", freq,
   //           g_value_get_float (mag));
	printf("%g %f\n", freq,
              g_value_get_float (mag));
        }
      }
    //  g_print ("\n");
	
    }
  }
  return TRUE;
}

int
main (int argc, char *argv[])
{
  GstElement *bin;
  GstElement *alsasrc, *audioconvert, *spectrum, *sink, *mad, *src, *alsasink;
  GstElement *spec_queue, *alsa_queue, *tee;	
  GstBus *bus;
  GstCaps *caps;
  GMainLoop *loop;

  gst_init (&argc, &argv);

  bin = gst_pipeline_new ("bin");

  src = gst_element_factory_make ("audiotestsrc", "src");
  g_object_set (G_OBJECT (src), "wave", 0, "freq", 2000.0, NULL);
	
	tee = gst_element_factory_make ("tee","tee");
	g_assert (tee);
	spec_queue = gst_element_factory_make("queue" , "spec_queue");
	g_assert (spec_queue);
	alsa_queue = gst_element_factory_make("queue" , "alsa_queue");
	g_assert (alsa_queue);
  	
	alsasink = gst_element_factory_make ("alsasink", NULL);
	g_assert ( alsasink);

	alsasrc = gst_element_factory_make ("alsasrc", NULL);
	g_assert ( alsasrc);

	mad = gst_element_factory_make ("mad", NULL);
	g_assert (mad);


  audioconvert = gst_element_factory_make ("audioconvert", NULL);
  g_assert (audioconvert);

  spectrum = gst_element_factory_make ("spectrum", "spectrum");
  g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -60,
      "message", TRUE, "message-phase", TRUE, NULL);

  sink = gst_element_factory_make ("fakesink", "sink");
  g_object_set (G_OBJECT (sink), "sync", TRUE, NULL);

  //gst_bin_add_many (GST_BIN (bin), src, audioconvert, spectrum, sink, NULL);
	//gst_bin_add_many (GST_BIN (bin), alsasrc, audioconvert, spectrum, sink, NULL);
	gst_bin_add_many (GST_BIN (bin), alsasrc, audioconvert, tee, alsa_queue, alsasink, spec_queue, spectrum, sink, NULL);

  caps = gst_caps_new_simple ("audio/x-raw-int",
      "rate", G_TYPE_INT, AUDIOFREQ, NULL);

  if (//!gst_element_link (src, audioconvert) ||
	!gst_element_link (alsasrc, audioconvert) ||
	!gst_element_link (audioconvert, tee) ||
	!gst_element_link (tee, alsa_queue) ||
	!gst_element_link (alsa_queue, alsasink ) ||
	!gst_element_link (tee, spec_queue ) ||
      !gst_element_link_filtered (spec_queue, spectrum, caps) ||
      !gst_element_link (spectrum, sink)) {
    fprintf (stderr, "can't link elements\n");
    exit (1);
  }
  gst_caps_unref (caps);

  bus = gst_element_get_bus (bin);
  gst_bus_add_watch (bus, message_handler, NULL);
  gst_object_unref (bus);

  gst_element_set_state (bin, GST_STATE_PLAYING);

  /* we need to run a GLib main loop to get the messages */
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  gst_element_set_state (bin, GST_STATE_NULL);

  gst_object_unref (bin);

  return 0;
}

