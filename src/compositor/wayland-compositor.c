// gcc -o wayland-compositor wayland-compositor.c backend-x11.c xdg-shell.c -lwayland-server -lX11 -lEGL -lGL -lX11-xcb -lxkbcommon-x11 -lxkbcommon

#include <clutter/clutter.h>
#include <clutter/wayland/clutter-wayland-compositor.h>
#include <clutter/wayland/clutter-wayland-surface.h>

#include <string.h>
#include <glib.h>

#include <wayland-server.h>
#include "xdg-shell-unstable-v5-server-protocol.h"
#include "xdg-shell-unstable-v6-server-protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


long backend_get_timestamp (void) {
	struct timespec t;
	clock_gettime (CLOCK_MONOTONIC, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

void info(int line, char *func, char *message) {
	printf("%s:%d %s\n", func, line, message);
}

void trace(int line, const char *func) {
	printf("%s:%d\n", func, line);
}

#define T // trace(__LINE__, __func__)
#define TF trace(__LINE__, __func__)

static struct wl_display *display;
static int pointer_x, pointer_y;
static struct modifier_state modifier_state;
static char redraw_needed = 0;
static ClutterActor *stage = NULL;
static ClutterActor *scroll = NULL;

struct client {
	struct wl_client *client;
	struct wl_resource *pointer;
	struct wl_resource *keyboard;
	struct wl_list link;
};
static struct wl_list clients;

static struct client *get_client (struct wl_client *_client) {
	struct client *client;
	wl_list_for_each (client, &clients, link) {
		if (client->client == _client) return client;
	}
	client = calloc (1, sizeof(struct client));
	client->client = _client;
	wl_list_insert (&clients, &client->link);
	return client;
}

struct surface {
	struct wl_resource *surface;
	struct wl_resource *xdg_surface;
	struct wl_resource *buffer;
	struct wl_resource *pending_buffer;
	struct wl_resource *frame_callback;
	int x, y;
	ClutterActor *actor;
	struct client *client;
	struct wl_list link;
};
static struct wl_list surfaces;
static struct surface *cursor = NULL;
static struct surface *moving_surface = NULL;
static struct surface *active_surface = NULL;
static struct surface *pointer_surface = NULL; // surface under the pointer

static void delete_surface (struct wl_resource *resource) {
	struct surface *surface = wl_resource_get_user_data (resource);
	wl_list_remove (&surface->link);
	if (surface == active_surface) active_surface = NULL;
	if (surface == pointer_surface) pointer_surface = NULL;
	free (surface);
	redraw_needed = 1;
}

// surface
static void surface_destroy (struct wl_client *client, struct wl_resource *resource) {
	
}
static void surface_attach (struct wl_client *client, struct wl_resource *resource, struct wl_resource *buffer, int32_t x, int32_t y) {
	struct surface *surface = wl_resource_get_user_data (resource);
	surface->pending_buffer = buffer;
}
static void surface_damage (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height) {
	struct surface *surface = wl_resource_get_user_data (resource);
	
	clutter_wayland_surface_damage_buffer((ClutterWaylandSurface*) surface->actor, surface->pending_buffer,
										  x, y, width, height);
	
}


static gboolean frame_done_cb(gpointer data)
{
	struct surface *surface = (struct surface*)data;
	TF;

	return FALSE;
}


static void surface_frame (struct wl_client *client, struct wl_resource *resource, uint32_t callback) {
	TF;
	struct surface *surface = wl_resource_get_user_data (resource);
	surface->frame_callback = wl_resource_create (client, &wl_callback_interface, 1, callback);
}
static void surface_set_opaque_region (struct wl_client *client, struct wl_resource *resource, struct wl_resource *region) {
	
}
static void surface_set_input_region (struct wl_client *client, struct wl_resource *resource, struct wl_resource *region) {
	
}

static void surface_commit (struct wl_client *client, struct wl_resource *resource) {
	struct surface *surface = wl_resource_get_user_data (resource);

	GError *error; 

	ClutterActor *actor = surface->actor;
	clutter_wayland_surface_attach_buffer((ClutterWaylandSurface*) actor, surface->pending_buffer, &error);

	if(surface->buffer != NULL)
		wl_buffer_send_release (surface->buffer);

	surface->buffer = surface->pending_buffer;

	TF;

	/* redraw_needed = 1; */
}
static void surface_set_buffer_transform (struct wl_client *client, struct wl_resource *resource, int32_t transform) {
	
}
static void surface_set_buffer_scale (struct wl_client *client, struct wl_resource *resource, int32_t scale) {
	
}
static struct wl_surface_interface surface_interface = {&surface_destroy,
 &surface_attach,
 &surface_damage,
 &surface_frame,
 &surface_set_opaque_region,
 &surface_set_input_region,
 &surface_commit,
 &surface_set_buffer_transform,
 &surface_set_buffer_scale};

// region
static void region_destroy (struct wl_client *client, struct wl_resource *resource) {
	
}
static void region_add (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height) {
	
}
static void region_subtract (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height) {
	
}
static struct wl_region_interface region_interface = {&region_destroy, &region_add, &region_subtract};

// compositor
static void compositor_create_surface (struct wl_client *client, struct wl_resource *resource, uint32_t id) {
	struct surface *surface = calloc (1, sizeof(struct surface));
	surface->buffer = NULL;
	surface->frame_callback = NULL;

	surface->surface = wl_resource_create (client, &wl_surface_interface, 3, id);
	surface->actor = clutter_wayland_surface_new((struct wl_surface*) surface->surface);
	clutter_actor_add_child(scroll, surface->actor);

	wl_resource_set_implementation (surface->surface, &surface_interface, surface, &delete_surface);
	surface->client = get_client (client);
	wl_list_insert (&surfaces, &surface->link);
}
static void compositor_create_region (struct wl_client *client, struct wl_resource *resource, uint32_t id) {
	struct wl_resource *region = wl_resource_create (client, &wl_region_interface, 1, id);
	wl_resource_set_implementation (region, &region_interface, NULL, NULL);
}
static struct wl_compositor_interface compositor_interface = {
	&compositor_create_surface,
	&compositor_create_region
};
static void compositor_bind (struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	printf ("bind: compositor\n");
	struct wl_resource *resource = wl_resource_create (client, &wl_compositor_interface, 1, id);
	wl_resource_set_implementation (resource, &compositor_interface, NULL, NULL);
}

// shell surface
static void shell_surface_pong (struct wl_client *client, struct wl_resource *resource, uint32_t serial) {
	
}
static void shell_surface_move (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial) {
	
}
static void shell_surface_resize (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, uint32_t edges) {
	
}	       
static void shell_surface_set_toplevel (struct wl_client *client, struct wl_resource *resource) {
	
}
static void shell_surface_set_transient (struct wl_client *client, struct wl_resource *resource, struct wl_resource *parent, int32_t x, int32_t y, uint32_t flags) {
	
}
static void shell_surface_set_fullscreen (struct wl_client *client, struct wl_resource *resource, uint32_t method, uint32_t framerate, struct wl_resource *output) {
	
}
static void shell_surface_set_popup (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, struct wl_resource *parent, int32_t x, int32_t y, uint32_t flags) {
	
}
static void shell_surface_set_maximized (struct wl_client *client, struct wl_resource *resource, struct wl_resource *output) {
	
}
static void shell_surface_set_title (struct wl_client *client, struct wl_resource *resource, const char *title) {
	
}
static void shell_surface_set_class (struct wl_client *client, struct wl_resource *resource, const char *class_) {
	
}
static struct wl_shell_surface_interface shell_surface_interface = {&shell_surface_pong, &shell_surface_move, &shell_surface_resize, &shell_surface_set_toplevel, &shell_surface_set_transient, &shell_surface_set_fullscreen, &shell_surface_set_popup, &shell_surface_set_maximized, &shell_surface_set_title, &shell_surface_set_class};

// wl shell
static void shell_get_shell_surface (struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface) {
	struct wl_resource *shell_surface = wl_resource_create (client, &wl_shell_surface_interface, 1, id);
	wl_resource_set_implementation (shell_surface, &shell_surface_interface, NULL, NULL);
}
static struct wl_shell_interface shell_interface = {&shell_get_shell_surface};
static void shell_bind (struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	printf ("bind: wl_shell\n");
	struct wl_resource *resource = wl_resource_create (client, &wl_shell_interface, 1, id);
	wl_resource_set_implementation (resource, &shell_interface, NULL, NULL);
}

// xdg surface
static void xdg_surface_destroy (struct wl_client *client, struct wl_resource *resource) {
	
}
static void xdg_surface_set_parent (struct wl_client *client, struct wl_resource *resource, struct wl_resource *parent) {
	
}
static void xdg_surface_set_title (struct wl_client *client, struct wl_resource *resource, const char *title) {
	
}
static void xdg_surface_set_app_id (struct wl_client *client, struct wl_resource *resource, const char *app_id) {
	
}
static void xdg_surface_show_window_menu (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, int32_t x, int32_t y) {
	
}
static void xdg_surface_move (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial) {
	struct surface *surface = wl_resource_get_user_data (resource);
	// during the move the surface coordinates are relative to the pointer
	surface->x = surface->x - pointer_x;
	surface->y = surface->y - pointer_y;
	moving_surface = surface;
}
static void xdg_surface_resize (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, uint32_t edges) {
	
}
static void xdg_surface_ack_configure (struct wl_client *client, struct wl_resource *resource, uint32_t serial) {
	
}
static void xdg_surface_set_window_geometry (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height) {
	
}
static void xdg_surface_set_maximized (struct wl_client *client, struct wl_resource *resource) {
	printf ("surface requested maximize\n");
}
static void xdg_surface_unset_maximized (struct wl_client *client, struct wl_resource *resource) {
	
}
static void xdg_surface_set_fullscreen (struct wl_client *client, struct wl_resource *resource, struct wl_resource *output) {
	
}
static void xdg_surface_unset_fullscreen (struct wl_client *client, struct wl_resource *resource) {
	
}
static void xdg_surface_set_minimized (struct wl_client *client, struct wl_resource *resource) {
	
}
static struct xdg_surface_interface my_xdg_surface_interface = {
	&xdg_surface_destroy,
	&xdg_surface_set_parent,
	&xdg_surface_set_title,
	&xdg_surface_set_app_id,
	&xdg_surface_show_window_menu,
	&xdg_surface_move,
	&xdg_surface_resize,
	&xdg_surface_ack_configure,
	&xdg_surface_set_window_geometry,
	&xdg_surface_set_maximized,
	&xdg_surface_unset_maximized,
	&xdg_surface_set_fullscreen,
	&xdg_surface_unset_fullscreen,
	&xdg_surface_set_minimized
};


void zxdg_surface_get_toplevel_impl(struct wl_client *client,
						   struct wl_resource *resource,
						   uint32_t id) {
	
}

void zxdg_surface_ack_configure_impl(struct wl_client *client,
							struct wl_resource *resource,
							uint32_t serial) {
	
}

void zxdg_surface_set_window_geometry_impl(struct wl_client *client,
								  struct wl_resource *resource,
								  int32_t x,
								  int32_t y,
								  int32_t width,
								  int32_t height) {
	
}

void zxdg_surface_get_popup_impl(struct wl_client *client,
						struct wl_resource *resource,
						uint32_t id,
						struct wl_resource *parent,
						struct wl_resource *positioner) {
	
}

void zxdg_surface_destroy_impl(struct wl_client *client,
					  struct wl_resource *resource) {
	
}

static struct zxdg_surface_v6_interface zxdg_surface_v6_interface_impl = {
	/**
	 * destroy the xdg_surface
	 *
	 * Destroy the xdg_surface object. An xdg_surface must only be
	 * destroyed after its role object has been destroyed.
	 */
	.destroy = zxdg_surface_destroy_impl,
	/**
	 * assign the xdg_toplevel surface role
	 *
	 * This creates an xdg_toplevel object for the given xdg_surface
	 * and gives the associated wl_surface the xdg_toplevel role.
	 *
	 * See the documentation of xdg_toplevel for more details about
	 * what an xdg_toplevel is and how it is used.
	 */
	.get_toplevel = zxdg_surface_get_toplevel_impl,
	/**
	 * assign the xdg_popup surface role
	 *
	 * This creates an xdg_popup object for the given xdg_surface and
	 * gives the associated wl_surface the xdg_popup role.
	 *
	 * See the documentation of xdg_popup for more details about what
	 * an xdg_popup is and how it is used.
	 */
	.get_popup = zxdg_surface_get_popup_impl,
	/**
	 * set the new window geometry
	 *
	 * The window geometry of a surface is its "visible bounds" from
	 * the user's perspective. Client-side decorations often have
	 * invisible portions like drop-shadows which should be ignored for
	 * the purposes of aligning, placing and constraining windows.
	 *
	 * The window geometry is double buffered, and will be applied at
	 * the time wl_surface.commit of the corresponding wl_surface is
	 * called.
	 *
	 * Once the window geometry of the surface is set, it is not
	 * possible to unset it, and it will remain the same until
	 * set_window_geometry is called again, even if a new subsurface or
	 * buffer is attached.
	 *
	 * If never set, the value is the full bounds of the surface,
	 * including any subsurfaces. This updates dynamically on every
	 * commit. This unset is meant for extremely simple clients.
	 *
	 * The arguments are given in the surface-local coordinate space of
	 * the wl_surface associated with this xdg_surface.
	 *
	 * The width and height must be greater than zero. Setting an
	 * invalid size will raise an error. When applied, the effective
	 * window geometry will be the set window geometry clamped to the
	 * bounding rectangle of the combined geometry of the surface of
	 * the xdg_surface and the associated subsurfaces.
	 */
	.set_window_geometry = zxdg_surface_set_window_geometry_impl,
	/**
	 * ack a configure event
	 *
	 * When a configure event is received, if a client commits the
	 * surface in response to the configure event, then the client must
	 * make an ack_configure request sometime before the commit
	 * request, passing along the serial of the configure event.
	 *
	 * For instance, for toplevel surfaces the compositor might use
	 * this information to move a surface to the top left only when the
	 * client has drawn itself for the maximized or fullscreen state.
	 *
	 * If the client receives multiple configure events before it can
	 * respond to one, it only has to ack the last configure event.
	 *
	 * A client is not required to commit immediately after sending an
	 * ack_configure request - it may even ack_configure several times
	 * before its next surface commit.
	 *
	 * A client may send multiple ack_configure requests before
	 * committing, but only the last request sent before a commit
	 * indicates which configure event the client really is responding
	 * to.
	 * @param serial the serial from the configure event
	 */
	.ack_configure = zxdg_surface_ack_configure_impl

};



/**
* destroy xdg_shell
*
* Destroy this xdg_shell object.
*
* Destroying a bound xdg_shell object while there are surfaces
* still alive created by this xdg_shell object instance is illegal
* and will result in a protocol error.
*/
void zxdg_shell_destroy_impl(struct wl_client *client,
				struct wl_resource *resource) {
}
/**
* create a positioner object
*
* Create a positioner object. A positioner object is used to
* position surfaces relative to some parent surface. See the
* interface description and xdg_surface.get_popup for details.
*/
void zxdg_shell_create_positioner_impl(struct wl_client *client,
				struct wl_resource *resource,
				uint32_t id) {
}
/**
* create a shell surface from a surface
*
* This creates an xdg_surface for the given surface. While
* xdg_surface itself is not a role, the corresponding surface may
* only be assigned a role extending xdg_surface, such as
* xdg_toplevel or xdg_popup.
*
* This creates an xdg_surface for the given surface. An
* xdg_surface is used as basis to define a role to a given
* surface, such as xdg_toplevel or xdg_popup. It also manages
* functionality shared between xdg_surface based surface roles.
*
* See the documentation of xdg_surface for more details about what
* an xdg_surface is and how it is used.
*/
void zxdg_shell_get_xdg_surface_impl(struct wl_client *client,
		struct wl_resource *resource,
		uint32_t id,
		struct wl_resource *surface) {
}
/**
* respond to a ping event
*
* A client must respond to a ping event with a pong request or
* the client may be deemed unresponsive. See xdg_shell.ping.
* @param serial serial of the ping event
*/
void zxdg_shell_pong_impl(struct wl_client *client,
		struct wl_resource *resource,
		uint32_t serial) {
}

static struct zxdg_shell_v6_interface zxdg_shell_v6_interface_impl = {
	.destroy = zxdg_shell_destroy_impl,
	.create_positioner = zxdg_shell_create_positioner_impl,
	.get_xdg_surface = zxdg_shell_get_xdg_surface_impl,
	.pong = zxdg_shell_pong_impl
};

// --------------------------

// xdg shell
static void xdg_shell_destroy (struct wl_client *client, struct wl_resource *resource) {
	
}
static void xdg_shell_use_unstable_version (struct wl_client *client, struct wl_resource *resource, int32_t version) {
	
}
static void xdg_shell_get_xdg_surface (struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *_surface) {
	struct surface *surface = wl_resource_get_user_data (_surface);
	surface->xdg_surface = wl_resource_create (client, &xdg_surface_interface, 1, id);
	wl_resource_set_implementation (surface->xdg_surface, &my_xdg_surface_interface, surface, NULL);
}
static void xdg_shell_get_xdg_popup (struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface, struct wl_resource *parent, struct wl_resource *seat, uint32_t serial, int32_t x, int32_t y) {
	
}
static void xdg_shell_pong (struct wl_client *client, struct wl_resource *resource, uint32_t serial) {
	
}
static struct xdg_shell_interface my_xdg_shell_interface = {
	&xdg_shell_destroy,
	&xdg_shell_use_unstable_version,
	&xdg_shell_get_xdg_surface,
	&xdg_shell_get_xdg_popup,
	&xdg_shell_pong
};
static void xdg_shell_bind (struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	printf ("bind: xdg_shell\n");
	struct wl_resource *resource = wl_resource_create (client, &xdg_shell_interface, 1, id);
	wl_resource_set_implementation (resource, &my_xdg_shell_interface, NULL, NULL);
}

// pointer
static void pointer_set_cursor (struct wl_client *client, struct wl_resource *resource, uint32_t serial, struct wl_resource *_surface, int32_t hotspot_x,
int32_t hotspot_y) {
	struct surface *surface = wl_resource_get_user_data (_surface);
	cursor = surface;
}
static void pointer_release (struct wl_client *client, struct wl_resource *resource) {
	
}
static struct wl_pointer_interface pointer_interface = {&pointer_set_cursor, &pointer_release};

// keyboard
static void keyboard_release (struct wl_client *client, struct wl_resource *resource) {
	
}
static struct wl_keyboard_interface keyboard_interface = {&keyboard_release};

// seat
static void seat_get_pointer (struct wl_client *client, struct wl_resource *resource, uint32_t id) {
	struct wl_resource *pointer = wl_resource_create (client, &wl_pointer_interface, 1, id);
	wl_resource_set_implementation (pointer, &pointer_interface, NULL, NULL);
	get_client(client)->pointer = pointer;
}
static void seat_get_keyboard (struct wl_client *client, struct wl_resource *resource, uint32_t id) {
	struct wl_resource *keyboard = wl_resource_create (client, &wl_keyboard_interface, 1, id);
	wl_resource_set_implementation (keyboard, &keyboard_interface, NULL, NULL);
	get_client(client)->keyboard = keyboard;
	int fd, size;
	/* backend_get_keymap (&fd, &size); */
	wl_keyboard_send_keymap (keyboard, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, fd, size);
	//close (fd);
}
static void seat_get_touch (struct wl_client *client, struct wl_resource *resource, uint32_t id) {
	
}
static struct wl_seat_interface seat_interface = {&seat_get_pointer, &seat_get_keyboard, &seat_get_touch};
static void seat_bind (struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	printf ("bind: seat\n");
	struct wl_resource *seat = wl_resource_create (client, &wl_seat_interface, 1, id);
	wl_resource_set_implementation (seat, &seat_interface, NULL, NULL);
	wl_seat_send_capabilities (seat, WL_SEAT_CAPABILITY_POINTER|WL_SEAT_CAPABILITY_KEYBOARD);
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

void after_paint(ClutterStage *stage, gpointer data)
{
	TF;
	struct surface *surface;
	wl_list_for_each(surface, &surfaces, link) {
		if (surface->frame_callback) {
			TF;
			wl_callback_send_done (surface->frame_callback, backend_get_timestamp());
			wl_resource_destroy (surface->frame_callback);
			surface->frame_callback = NULL;
		}
	}

}


int main () {


	wl_list_init (&clients);
	wl_list_init (&surfaces);
	display = wl_display_create ();


	GSource *wayland_event_source;

	// wayland event loop
	wayland_event_source = wayland_event_source_new (display);
	g_source_attach (wayland_event_source, NULL);

	clutter_wayland_set_compositor_display(display);

	wl_display_add_socket_auto (display);
	wl_global_create (display, &wl_compositor_interface, 3, NULL, &compositor_bind);
	wl_global_create (display, &wl_shell_interface, 1, NULL, &shell_bind);
	wl_global_create (display, &xdg_shell_interface, 1, NULL, &xdg_shell_bind);
	wl_global_create (display, &wl_seat_interface, 1, NULL, &seat_bind);
	wl_display_init_shm (display);

	// stage
	// clutter
	// gjs

	clutter_init(0, NULL);
	stage = clutter_stage_new ();
	/* clutter_stage_set_user_resizable(stage, TRUE); */
	clutter_actor_show(stage);

	scroll = clutter_scroll_actor_new();
	clutter_actor_add_child(stage,scroll);
	clutter_actor_set_layout_manager(scroll, clutter_box_layout_new());

	GMainLoop *loop = g_main_loop_new (NULL, FALSE);

	g_signal_connect(stage, "after-paint", G_CALLBACK(after_paint), loop);

	g_main_loop_run (loop);

	wl_display_destroy (display);
	return 0;
}
