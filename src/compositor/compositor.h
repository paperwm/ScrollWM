 
#include <clutter/clutter.h>
#include <clutter/wayland/clutter-wayland.h>
#include <clutter/wayland/clutter-wayland-compositor.h>
#include <clutter/wayland/clutter-wayland-surface.h>

#include <wayland-server.h>

#include <string.h>
#include <glib.h>

#include <wayland-server.h>
#include "xdg-shell-unstable-v5-server-protocol.h"
#include "xdg-shell-unstable-v6-server-protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


extern struct wl_display *display;
extern ClutterActor *stage;
extern ClutterActor *scroll;

struct surface {
    struct wl_resource *surface;
    struct wl_resource *xdg_surface;
    struct wl_resource *xdg_toplevel_surface;
    struct wl_resource *buffer;
    struct wl_resource *pending_buffer;
    struct wl_resource *frame_callback;
    int x, y;
    ClutterActor *actor;
    struct client *client;
    struct wl_list link;
};

struct client {
    struct wl_client *client;
    struct wl_resource *pointer;
    struct wl_resource *keyboard;
    struct wl_list link;
};

extern void xdg_bind_init();
