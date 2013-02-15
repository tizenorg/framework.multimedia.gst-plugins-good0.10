/* GStreamer
 * (c) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * (c) 2005 Tim-Philipp Müller <tim centricular net>
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
/**
 * SECTION:element-gconfaudiosrc
 * @see_also: #GstAlsaSrc, #GstAutoAudioSrc
 *
 * This element records sound from the audiosink that has been configured in
 * GConf by the user.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch gconfaudiosrc ! audioconvert ! wavenc ! filesink location=record.wav
 * ]| Record from configured audioinput
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "gstgconfelements.h"
#include "gstgconfaudiosrc.h"

static void gst_gconf_audio_src_dispose (GObject * object);
static void gst_gconf_audio_src_finalize (GstGConfAudioSrc * src);
static void cb_toggle_element (GConfClient * client,
    guint connection_id, GConfEntry * entry, gpointer data);
static GstStateChangeReturn
gst_gconf_audio_src_change_state (GstElement * element,
    GstStateChange transition);

GST_BOILERPLATE (GstGConfAudioSrc, gst_gconf_audio_src, GstSwitchSrc,
    GST_TYPE_SWITCH_SRC);

static void
gst_gconf_audio_src_base_init (gpointer klass)
{
  GstElementClass *eklass = GST_ELEMENT_CLASS (klass);

  gst_element_class_set_details_simple (eklass, "GConf audio source",
      "Source/Audio",
      "Audio source embedding the GConf-settings for audio input",
      "GStreamer maintainers <gstreamer-devel@lists.sourceforge.net>");
}

static void
gst_gconf_audio_src_class_init (GstGConfAudioSrcClass * klass)
{
  GObjectClass *oklass = G_OBJECT_CLASS (klass);
  GstElementClass *eklass = GST_ELEMENT_CLASS (klass);

  oklass->dispose = gst_gconf_audio_src_dispose;
  oklass->finalize = (GObjectFinalizeFunc) gst_gconf_audio_src_finalize;
  eklass->change_state = gst_gconf_audio_src_change_state;
}

/*
 * Hack to make negotiation work.
 */

static gboolean
gst_gconf_audio_src_reset (GstGConfAudioSrc * src)
{
  gst_switch_src_set_child (GST_SWITCH_SRC (src), NULL);

  g_free (src->gconf_str);
  src->gconf_str = NULL;
  return TRUE;
}

static void
gst_gconf_audio_src_init (GstGConfAudioSrc * src,
    GstGConfAudioSrcClass * g_class)
{
  gst_gconf_audio_src_reset (src);

  src->client = gconf_client_get_default ();
  gconf_client_add_dir (src->client, GST_GCONF_DIR,
      GCONF_CLIENT_PRELOAD_RECURSIVE, NULL);
  src->gconf_notify_id = gconf_client_notify_add (src->client,
      GST_GCONF_DIR "/" GST_GCONF_AUDIOSRC_KEY,
      cb_toggle_element, src, NULL, NULL);
}

static void
gst_gconf_audio_src_dispose (GObject * object)
{
  GstGConfAudioSrc *src = GST_GCONF_AUDIO_SRC (object);

  if (src->client) {
    if (src->gconf_notify_id) {
      gconf_client_notify_remove (src->client, src->gconf_notify_id);
      src->gconf_notify_id = 0;
    }

    g_object_unref (G_OBJECT (src->client));
    src->client = NULL;
  }

  GST_CALL_PARENT (G_OBJECT_CLASS, dispose, (object));
}

static void
gst_gconf_audio_src_finalize (GstGConfAudioSrc * src)
{
  g_free (src->gconf_str);

  GST_CALL_PARENT (G_OBJECT_CLASS, finalize, ((GObject *) (src)));
}

static gboolean
do_toggle_element (GstGConfAudioSrc * src)
{
  GstElement *new_kid;
  gchar *new_gconf_str;

  new_gconf_str = gst_gconf_get_string (GST_GCONF_AUDIOSRC_KEY);
  if (new_gconf_str != NULL && src->gconf_str != NULL &&
      (strlen (new_gconf_str) == 0 ||
          strcmp (src->gconf_str, new_gconf_str) == 0)) {
    g_free (new_gconf_str);
    GST_DEBUG_OBJECT (src, "GConf key was updated, but it didn't change");
    return TRUE;
  }

  GST_DEBUG_OBJECT (src, "GConf key changed: '%s' to '%s'",
      GST_STR_NULL (src->gconf_str), GST_STR_NULL (new_gconf_str));

  GST_DEBUG_OBJECT (src, "Creating new kid");
  if (!(new_kid = gst_gconf_get_default_audio_src ())) {
    GST_ELEMENT_ERROR (src, LIBRARY, SETTINGS, (NULL),
        ("Failed to render audio src from GConf"));
    return FALSE;
  }

  if (!gst_switch_src_set_child (GST_SWITCH_SRC (src), new_kid)) {
    GST_WARNING_OBJECT (src, "Failed to update child element");
    goto fail;
  }

  g_free (src->gconf_str);
  src->gconf_str = new_gconf_str;

  GST_DEBUG_OBJECT (src, "done changing gconf audio src");

  return TRUE;
fail:
  g_free (new_gconf_str);
  return FALSE;
}

static void
cb_toggle_element (GConfClient * client,
    guint connection_id, GConfEntry * entry, gpointer data)
{
  do_toggle_element (GST_GCONF_AUDIO_SRC (data));
}

static GstStateChangeReturn
gst_gconf_audio_src_change_state (GstElement * element,
    GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstGConfAudioSrc *src = GST_GCONF_AUDIO_SRC (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      if (!do_toggle_element (src)) {
        gst_gconf_audio_src_reset (src);
        return GST_STATE_CHANGE_FAILURE;
      }
      break;
    default:
      break;
  }

  ret = GST_CALL_PARENT_WITH_DEFAULT (GST_ELEMENT_CLASS, change_state,
      (element, transition), GST_STATE_CHANGE_SUCCESS);

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_NULL:
      if (!gst_gconf_audio_src_reset (src))
        ret = GST_STATE_CHANGE_FAILURE;
      break;
    default:
      break;
  }

  return ret;
}
