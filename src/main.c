
// clutter unfortunately doesn't ship with wayland support by default
#include <clutter/clutter.h>
#include <clutter/wayland/clutter-wayland-compositor.h>
#include <clutter/wayland/clutter-wayland-surface.h>

#include <wayland-server.h>

#include <glib.h>

#include <string.h>


struct scroll_compositor {
  struct wl_display *wl_display;
  struct wl_list frame_callbacks;
};


/* struct _MetaWaylandOutput */
/* { */
/*   GObject                   parent; */

/*   struct wl_global         *global; */
/*   MetaLogicalMonitor       *logical_monitor; */
/*   guint                     mode_flags; */
/*   float                     refresh_rate; */
/*   gint                      scale; */

/*   GList                    *resources; */
/* }; */

typedef struct scroll_compositor ScrollCompositor;

static int signal_pipe[2];

static void
compositor_bind (struct wl_client *client,
                 void *data,
                 guint32 version,
                 guint32 id)
{
  /* MetaWaylandCompositor *compositor = data; */
  struct wl_resource *resource;

  resource = wl_resource_create (client, &wl_compositor_interface, version, id);
  /* wl_resource_set_implementation (resource, &meta_wayland_wl_compositor_interface, compositor, NULL); */
}

typedef struct
{
  GSource source;
  struct wl_display *display;
} WaylandEventSource;

/*
 */
static gboolean
wayland_event_source_prepare (GSource *base, int *timeout)
{
  WaylandEventSource *source = (WaylandEventSource *)base;

  *timeout = -1;

  wl_display_flush_clients (source->display);

  return FALSE;
}

/*
 */
static gboolean
wayland_event_source_dispatch (GSource *base,
                               GSourceFunc callback,
                               void *data)
{
  WaylandEventSource *source = (WaylandEventSource *)base;
  struct wl_event_loop *loop = wl_display_get_event_loop (source->display);

  wl_event_loop_dispatch (loop, 0);

  return TRUE;
}

static GSourceFuncs wayland_event_source_funcs =
  {
    wayland_event_source_prepare,
    NULL,
    wayland_event_source_dispatch,
    NULL
  };

static GSource *
wayland_event_source_new (struct wl_display *display)
{
  // setup the wayland event loop
  WaylandEventSource *source;
  struct wl_event_loop *loop = wl_display_get_event_loop (display);

  source = (WaylandEventSource *) g_source_new (&wayland_event_source_funcs,
                                                sizeof (WaylandEventSource));
  source->display = display;
  g_source_add_unix_fd (&source->source,
                        wl_event_loop_get_fd (loop),
                        G_IO_IN | G_IO_ERR);

  return &source->source;
}

int
main (int argc, char **argv)
{
  GIOChannel *signal_reciever;
  struct sigaction signal_action;
  ScrollCompositor compositor;

  GSource *wayland_event_source;

  // wl_display
  struct wl_display *display = wl_display_create();

  // wayland event loop
  wayland_event_source = wayland_event_source_new (display);
  g_source_attach (wayland_event_source, NULL);

  // compositor interface
  wl_global_create (display,
                    &wl_compositor_interface,
                    3,
                    NULL, compositor_bind);

  wl_display_init_shm (display);

  // socket
  wl_display_add_socket_auto (display);

  /* set_gnome_env ("DISPLAY", meta_wayland_get_xwayland_display_name (compositor)); */
  /* set_gnome_env ("WAYLAND_DISPLAY", meta_wayland_get_wayland_display_name (compositor)); */

  // output init at least


  // stage
  // clutter
  // gjs

  g_main_loop_run (g_main_loop_new (NULL, FALSE));

}
