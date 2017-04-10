
// clutter unfortunately doesn't ship with wayland support by default
#include <clutter/clutter.h>
#include <clutter/wayland/clutter-wayland-compositor.h>
#include <clutter/wayland/clutter-wayland-surface.h>

#include <wayland-server.h>
#include "xdg-foreign.h"
#include "xdg-shell.h"

#include <glib.h>

#include <string.h>


void info(int line, char *func, char *message) {
  printf("%s:%d %s\n", func, line, message);
}

void trace(int line, const char *func) {
  printf("%s:%d\n", func, line);
}

#define T trace(__LINE__, __func__)

struct scroll_compositor {
  struct wl_display *wl_display;
  struct wl_list frame_callbacks;
};


typedef struct scroll_compositor ScrollCompositor;

static int signal_pipe[2];



static void
compositor_create_surface (struct wl_client *client,
                              struct wl_resource *resource,
                              guint32 id)
{
  /* MetaWaylandCompositor *compositor = wl_resource_get_user_data (resource); */
  struct wl_surface *surface = wl_resource_create (client, &wl_surface_interface, wl_resource_get_version (resource), id);
  ClutterActor *surface_actor = clutter_wayland_surface_new(surface);

  wl_resource_set_implementation (surface, &wl_surface_interface,
                                  surface_actor, wl_surface_destructor);
}

static const struct wl_compositor_interface meta_wayland_wl_compositor_interface = {
  .wl_compositor_create_surface = compositor_create_surface,
  .wl_compositor_create_region = compositor_create_region
};

static void
compositor_bind (struct wl_client *client,
                 void *data,
                 guint32 version,
                 guint32 id)
{
  /* MetaWaylandCompositor *compositor = data; */
  T;
  struct wl_resource *resource;

  resource = wl_resource_create (client, &wl_compositor_interface, version, id);
  wl_resource_set_implementation (resource, &wl_compositor_interface, NULL, NULL);
  T;
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
  T;
  WaylandEventSource *source = (WaylandEventSource *)base;

  *timeout = -1;

  wl_display_flush_clients (source->display);

  T;
  return FALSE;
}

/*
 */
static gboolean
wayland_event_source_dispatch (GSource *base,
                               GSourceFunc callback,
                               void *data)
{
  T;
  WaylandEventSource *source = (WaylandEventSource *)base;
  T;
  struct wl_event_loop *loop = wl_display_get_event_loop (source->display);

  T;
  wl_event_loop_dispatch (loop, 0);

  T;
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

static void
bind_xdg_exporter (struct wl_client *client,
                   void             *data,
                   uint32_t          version,
                   uint32_t          id)
{
  T;
  /* MetaWaylandXdgForeign *foreign = data; */
  struct wl_resource *resource;

  resource = wl_resource_create (client,
                                 &zxdg_exporter_v1_interface,
                                 1,
                                 id);

  if (resource == NULL)
    {
      wl_client_post_no_memory (client);
      return;
    }

  wl_resource_set_implementation (resource,
                                  &zxdg_exporter_v1_interface,
                                  NULL, NULL);
  T;
}

static void
bind_xdg_importer (struct wl_client *client,
                   void             *data,
                   uint32_t          version,
                   uint32_t          id)
{
  /* MetaWaylandXdgForeign *foreign = data; */
  struct wl_resource *resource;

  resource = wl_resource_create (client,
                                 &zxdg_importer_v1_interface,
                                 1,
                                 id);

  T;
  if (resource == NULL)
    {
      wl_client_post_no_memory (client);
      return;
    }

  wl_resource_set_implementation (resource,
                                  &zxdg_importer_v1_interface,
                                  NULL,
                                  NULL);
}


static void
bind_xdg_shell (struct wl_client *client,
                void             *data,
                guint32           version,
                guint32           id)
{
  /* MetaWaylandXdgShellClient *shell_client; */

  /* shell_client = g_new0 (MetaWaylandXdgShellClient, 1); */

  T;
  struct wl_resource *resource = wl_resource_create (client,
                                              &zxdg_shell_v6_interface,
                                              version, id);
  T;
  wl_resource_set_implementation (resource,
                                  &zxdg_shell_v6_interface,
                                  NULL, wl_resource_destroy);
}

static void
bind_output (struct wl_client *client,
             void *data,
             guint32 version,
             guint32 id)
{
  /* MetaWaylandOutput *wayland_output = data; */
  /* MetaLogicalMonitor *logical_monitor = wayland_output->logical_monitor; */
  struct wl_resource *resource;
  /* MetaMonitor *monitor; */

  resource = wl_resource_create (client, &wl_output_interface, version, id);
  /* wayland_output->resources = g_list_prepend (wayland_output->resources, resource); */

  /* wl_resource_set_user_data (resource, wayland_output); */
  wl_resource_set_destructor (resource, wl_resource_destroy);

  /* monitor = pick_main_monitor (logical_monitor); */

  /* meta_verbose ("Binding monitor %p/%s (%u, %u, %u, %u) x %f\n", */
  /*               logical_monitor, */
  /*               meta_monitor_get_product (monitor), */
  /*               logical_monitor->rect.x, logical_monitor->rect.y, */
  /*               logical_monitor->rect.width, logical_monitor->rect.height, */
  /*               wayland_output->refresh_rate); */

  wl_output_send_geometry (resource,
                           0,
                           0,
                           100,
                           80,
                           WL_OUTPUT_SUBPIXEL_VERTICAL_BGR,
                           "bar",
                           "aoeu",
                           WL_OUTPUT_TRANSFORM_NORMAL);
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
  clutter_wayland_set_compositor_display(display);

  // wayland event loop
  wayland_event_source = wayland_event_source_new (display);
  g_source_attach (wayland_event_source, NULL);

  // compositor interface
  wl_global_create (display,
                    &wl_compositor_interface,
                    3,
                    NULL, compositor_bind);

  wl_global_create (display,
                    &wl_output_interface,
                    2,
                    NULL, bind_output);

  // no idea what this is
  wl_global_create (display,
                    &zxdg_exporter_v1_interface, 1,
                    NULL,
                    bind_xdg_exporter);
  wl_global_create (display,
                    &zxdg_importer_v1_interface, 1,
                    NULL,
                    bind_xdg_importer);
  // xdg_shell
  wl_global_create (display,
                    &zxdg_shell_v6_interface,
                    1,
                    NULL,
                    bind_xdg_shell);

  wl_display_init_shm (display);


  /* meta_wayland_xdg_foreign_init (compositor); */

  // socket
  wl_display_add_socket_auto (display);

  /* meta_wayland_xdg_foreign_init (compositor); */

  /* set_gnome_env ("DISPLAY", meta_wayland_get_xwayland_display_name (compositor)); */
  /* set_gnome_env ("WAYLAND_DISPLAY", meta_wayland_get_wayland_display_name (compositor)); */

  // output init at least


  // stage
  // clutter
  // gjs

  clutter_init(0, NULL);
  ClutterActor *stage = clutter_stage_new ();
  /* clutter_stage_set_user_resizable(stage, TRUE); */
  clutter_actor_show_all (stage);

  g_main_loop_run (g_main_loop_new (NULL, FALSE));

}
