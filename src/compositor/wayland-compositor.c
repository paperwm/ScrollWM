// gcc -o wayland-compositor wayland-compositor.c backend-x11.c xdg-shell.c -lwayland-server -lX11 -lEGL -lGL -lX11-xcb -lxkbcommon-x11 -lxkbcommon

#include "compositor.h"

void info(int line, char *func, char *message) {
	printf("%s:%d %s\n", func, line, message);
}

void trace(int line, const char *func) {
	printf("%s:%d\n", func, line);
}

#define T // trace(__LINE__, __func__)
#define TF trace(__LINE__, __func__)

static struct wl_list clients;
static struct wl_list surfaces;

struct wl_display *display;
struct wl_seat *seat;
static int pointer_x, pointer_y;
static struct modifier_state modifier_state;
static char redraw_needed = 0;
static ClutterActor *stage = NULL;
ClutterActor *scroll = NULL;

static struct
client *get_client (struct wl_client *_client) {
	struct client *client;
	wl_list_for_each (client, &clients, link) {
		if (client->client == _client) return client;
	}
	client = calloc (1, sizeof(struct client));
	client->client = _client;
	wl_list_insert (&clients, &client->link);
	return client;
}

static struct surface *cursor = NULL;
static struct surface *moving_surface = NULL;
static struct surface *active_surface = NULL;
static struct surface *pointer_surface = NULL; // surface under the pointer

static void
delete_surface(struct wl_resource *resource) {
	struct surface *surface = wl_resource_get_user_data (resource);
	clutter_actor_destroy(surface->actor);
	wl_list_remove (&surface->link);
	if (surface == active_surface) active_surface = NULL;
	if (surface == pointer_surface) pointer_surface = NULL;
	free (surface);
	redraw_needed = 1;
}

// surface
static void
surface_destroy(struct wl_client *client,
				struct wl_resource *resource) {

}

static void
surface_attach(struct wl_client *client,
			   struct wl_resource *resource,
			   struct wl_resource *buffer,
			   int32_t x,
			   int32_t y) {
	struct surface *surface = wl_resource_get_user_data (resource);
	surface->pending_buffer = buffer;
}

static void
surface_damage(struct wl_client *client,
			   struct wl_resource *resource,
			   int32_t x,
			   int32_t y,
			   int32_t width,
			   int32_t height) {
	struct surface *surface = wl_resource_get_user_data (resource);

	clutter_wayland_surface_damage_buffer((ClutterWaylandSurface*) surface->actor, surface->pending_buffer,
										  x, y, width, height);

}


static gboolean
frame_done_cb(gpointer data)
{
	struct surface *surface = (struct surface*)data;
	TF;

	return FALSE;
}


static void
surface_frame(struct wl_client *client,
			  struct wl_resource *resource,
			  uint32_t callback) {
	TF;
	struct surface *surface = wl_resource_get_user_data (resource);
	surface->frame_callback = wl_resource_create (client, &wl_callback_interface, 1, callback);
}

static void
surface_set_opaque_region(struct wl_client *client,
						  struct wl_resource *resource,
						  struct wl_resource *region) {

}

static void
surface_set_input_region(struct wl_client *client,
						 struct wl_resource *resource,
						 struct wl_resource *region) {

}

static void
surface_commit(struct wl_client *client,
			   struct wl_resource *resource) {
	struct surface *surface = wl_resource_get_user_data (resource);
	// clients are able to send commit before attach
	if (surface->pending_buffer == NULL)
		return;

	GError *error;

	ClutterActor *actor = surface->actor;
	clutter_wayland_surface_attach_buffer((ClutterWaylandSurface*) actor, surface->pending_buffer, &error);

	if(surface->buffer != NULL)
		wl_buffer_send_release (surface->buffer);

	surface->buffer = surface->pending_buffer;

	/* redraw_needed = 1; */
}

static void
surface_set_buffer_transform(struct wl_client *client,
							 struct wl_resource *resource,
							 int32_t transform) {

}

static void
surface_set_buffer_scale(struct wl_client *client,
						 struct wl_resource *resource,
						 int32_t scale) {

}

static struct wl_surface_interface surface_interface = {
	&surface_destroy,
	&surface_attach,
	&surface_damage,
	&surface_frame,
	&surface_set_opaque_region,
	&surface_set_input_region,
	&surface_commit,
	&surface_set_buffer_transform,
	&surface_set_buffer_scale
};

// region
static void
region_destroy(struct wl_client *client,
			   struct wl_resource *resource) {

}

static void
region_add(struct wl_client *client,
		   struct wl_resource *resource,
		   int32_t x,
		   int32_t y,
		   int32_t width,
		   int32_t height) {

}

static void
region_subtract(struct wl_client *client,
				struct wl_resource *resource,
				int32_t x,
				int32_t y,
				int32_t width,
				int32_t height) {

}

static struct wl_region_interface region_interface = {&region_destroy, &region_add, &region_subtract};

static gboolean
enter_event(ClutterActor *actor,
			ClutterCrossingEvent *event,
			gpointer data) {

	struct client *client = data;
	printf("enter_event: %s (%.2f, %.2f)\n", clutter_actor_get_name(actor), event->x, event->y);
	if(client->pointer) {
		gfloat x = clutter_actor_get_x(actor);
		gfloat y = clutter_actor_get_y(actor);
		uint32_t serial = wl_display_next_serial(display);
		struct wl_surface *surface = clutter_wayland_surface_get_surface(actor);
		wl_pointer_send_enter(client->pointer, serial, surface,
							  wl_fixed_from_double(event->x - x),
							  wl_fixed_from_double(event->y - y));
		return TRUE;
	}
	return FALSE;
}


// compositor
static void
compositor_create_surface(struct wl_client *client,
						  struct wl_resource *resource,
						  uint32_t id) {
	struct surface *surface = calloc (1, sizeof(struct surface));
	surface->buffer = NULL;
	surface->frame_callback = NULL;

	surface->surface = wl_resource_create (client, &wl_surface_interface, 3, id);
	surface->actor = clutter_wayland_surface_new((struct wl_surface*) surface->surface);

	wl_resource_set_implementation (surface->surface, &surface_interface, surface, &delete_surface);
	surface->client = get_client (client);

	g_signal_connect(surface->actor, "enter-event", G_CALLBACK(enter_event), surface->client);

	wl_list_insert (&surfaces, &surface->link);
}

static void
compositor_create_region(struct wl_client *client,
						 struct wl_resource *resource,
						 uint32_t id) {
	struct wl_resource *region = wl_resource_create (client, &wl_region_interface, 1, id);
	wl_resource_set_implementation (region, &region_interface, NULL, NULL);
}

static struct wl_compositor_interface compositor_interface = {
	&compositor_create_surface,
	&compositor_create_region
};

static void
compositor_bind(struct wl_client *client,
				void *data,
				uint32_t version,
				uint32_t id) {
	printf ("bind: compositor\n");
	struct wl_resource *resource = wl_resource_create (client, &wl_compositor_interface, 1, id);
	wl_resource_set_implementation (resource, &compositor_interface, NULL, NULL);
}

// shell surface
static void
shell_surface_pong(struct wl_client *client,
				   struct wl_resource *resource,
				   uint32_t serial) {

}

static void
shell_surface_move(struct wl_client *client,
				   struct wl_resource *resource,
				   struct wl_resource *seat,
				   uint32_t serial) {

}

static void
shell_surface_resize(struct wl_client *client,
					 struct wl_resource *resource,
					 struct wl_resource *seat,
					 uint32_t serial,
					 uint32_t edges) {

}

static void
shell_surface_set_toplevel(struct wl_client *client,
						   struct wl_resource *resource) {

}

static void
shell_surface_set_transient(struct wl_client *client,
							struct wl_resource *resource,
							struct wl_resource *parent,
							int32_t x,
							int32_t y,
							uint32_t flags) {

}

static void
shell_surface_set_fullscreen(struct wl_client *client,
							 struct wl_resource *resource,
							 uint32_t method,
							 uint32_t framerate,
							 struct wl_resource *output) {

}

static void
shell_surface_set_popup(struct wl_client *client,
						struct wl_resource *resource,
						struct wl_resource *seat,
						uint32_t serial,
						struct wl_resource *parent,
						int32_t x,
						int32_t y,
						uint32_t flags) {

}

static void
shell_surface_set_maximized(struct wl_client *client,
							struct wl_resource *resource,
							struct wl_resource *output) {

}

static void
shell_surface_set_title(struct wl_client *client,
						struct wl_resource *resource,
						const char *title) {

}

static void
shell_surface_set_class(struct wl_client *client,
						struct wl_resource *resource,
						const char *class_) {

}

static struct wl_shell_surface_interface shell_surface_interface = {
	&shell_surface_pong,
	&shell_surface_move,
	&shell_surface_resize,
	&shell_surface_set_toplevel,
	&shell_surface_set_transient,
	&shell_surface_set_fullscreen,
	&shell_surface_set_popup,
	&shell_surface_set_maximized,
	&shell_surface_set_title,
	&shell_surface_set_class
};

// wl shell
static void
shell_get_shell_surface(struct wl_client *client,
						struct wl_resource *resource,
						uint32_t id,
						struct wl_resource *surface) {
	struct wl_resource *shell_surface = wl_resource_create (client, &wl_shell_surface_interface, 1, id);
	wl_resource_set_implementation (shell_surface, &shell_surface_interface, NULL, NULL);
}

static struct wl_shell_interface shell_interface = {&shell_get_shell_surface};

static void
shell_bind(struct wl_client *client,
		   void *data,
		   uint32_t version,
		   uint32_t id) {
	printf ("bind: wl_shell\n");
	struct wl_resource *resource = wl_resource_create (client, &wl_shell_interface, 1, id);
	wl_resource_set_implementation (resource, &shell_interface, NULL, NULL);
}

// xdg surface

static void
xdg_surface_destroy(struct wl_client *client,
					struct wl_resource *resource) {

}

static void
xdg_surface_set_parent(struct wl_client *client,
					   struct wl_resource *resource,
					   struct wl_resource *parent) {

}

static void
xdg_surface_set_title(struct wl_client *client,
					  struct wl_resource *resource,
					  const char *title) {

}

static void
xdg_surface_set_app_id(struct wl_client *client,
					   struct wl_resource *resource,
					   const char *app_id) {

}

static void
xdg_surface_show_window_menu(struct wl_client *client,
							 struct wl_resource *resource,
							 struct wl_resource *seat,
							 uint32_t serial,
							 int32_t x,
							 int32_t y) {

}

static void
xdg_surface_move(struct wl_client *client,
				 struct wl_resource *resource,
				 struct wl_resource *seat,
				 uint32_t serial) {
	struct surface *surface = wl_resource_get_user_data (resource);
	// during the move the surface coordinates are relative to the pointer
	surface->x = surface->x - pointer_x;
	surface->y = surface->y - pointer_y;
	moving_surface = surface;
}

static void
xdg_surface_resize(struct wl_client *client,
				   struct wl_resource *resource,
				   struct wl_resource *seat,
				   uint32_t serial,
				   uint32_t edges) {

}

static void
xdg_surface_ack_configure(struct wl_client *client,
						  struct wl_resource *resource,
						  uint32_t serial) {

}

static void
xdg_surface_set_window_geometry(struct wl_client *client,
								struct wl_resource *resource,
								int32_t x,
								int32_t y,
								int32_t width,
								int32_t height) {

}

static void
xdg_surface_set_maximized(struct wl_client *client,
						  struct wl_resource *resource) {
	printf ("surface requested maximize\n");
}

static void
xdg_surface_unset_maximized(struct wl_client *client,
							struct wl_resource *resource) {

}

static void
xdg_surface_set_fullscreen(struct wl_client *client,
						   struct wl_resource *resource,
						   struct wl_resource *output) {

}

static void
xdg_surface_unset_fullscreen(struct wl_client *client,
							 struct wl_resource *resource) {

}

static void
xdg_surface_set_minimized(struct wl_client *client,
						  struct wl_resource *resource) {

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


// xdg shell
static void
xdg_shell_destroy(struct wl_client *client,
				  struct wl_resource *resource) {

}

static void
xdg_shell_use_unstable_version(struct wl_client *client,
							   struct wl_resource *resource,
							   int32_t version) {

}

static void
xdg_shell_get_xdg_surface(struct wl_client *client,
						  struct wl_resource *resource,
						  uint32_t id,
						  struct wl_resource *_surface) {
	struct surface *surface = wl_resource_get_user_data (_surface);
	surface->xdg_surface = wl_resource_create (client, &xdg_surface_interface, 1, id);
	wl_resource_set_implementation (surface->xdg_surface, &my_xdg_surface_interface, surface, NULL);
}

static void
xdg_shell_get_xdg_popup(struct wl_client *client,
						struct wl_resource *resource,
						uint32_t id,
						struct wl_resource *surface,
						struct wl_resource *parent,
						struct wl_resource *seat,
						uint32_t serial,
						int32_t x,
						int32_t y) {

}

static void
xdg_shell_pong(struct wl_client *client,
			   struct wl_resource *resource,
			   uint32_t serial) {

}

static struct xdg_shell_interface my_xdg_shell_interface = {
	&xdg_shell_destroy,
	&xdg_shell_use_unstable_version,
	&xdg_shell_get_xdg_surface,
	&xdg_shell_get_xdg_popup,
	&xdg_shell_pong
};

static void
xdg_shell_bind(struct wl_client *client,
			   void *data,
			   uint32_t version,
			   uint32_t id) {
	printf("bind: xdg_shell\n");
	struct wl_resource *resource = wl_resource_create (client, &xdg_shell_interface, 1, id);
	wl_resource_set_implementation (resource, &my_xdg_shell_interface, NULL, NULL);
}

// pointer
static void
pointer_set_cursor(struct wl_client *client,
				   struct wl_resource *resource,
				   uint32_t serial,
				   struct wl_resource *_surface,
				   int32_t hotspot_x,
				   int32_t hotspot_y) {
	struct surface *surface = wl_resource_get_user_data (_surface);
	cursor = surface;
}

static void
pointer_release(struct wl_client *client,
				struct wl_resource *resource) {

}

static struct wl_pointer_interface pointer_interface = {
	&pointer_set_cursor,
	&pointer_release
};

// keyboard
static void
keyboard_release(struct wl_client *client,
				 struct wl_resource *resource) {

}

static struct wl_keyboard_interface keyboard_interface = { &keyboard_release };

// seat
static void
seat_get_pointer(struct wl_client *client,
				 struct wl_resource *resource,
				 uint32_t id) {
	struct wl_resource *pointer = wl_resource_create (client, &wl_pointer_interface, 1, id);
	wl_resource_set_implementation (pointer, &pointer_interface, NULL, NULL);
	get_client(client)->pointer = pointer;
}

static void
seat_get_keyboard(struct wl_client *client,
				  struct wl_resource *resource,
				  uint32_t id) {
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

static void
seat_bind(struct wl_client *client,
		  void *data,
		  uint32_t version,
		  uint32_t id) {
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
wayland_event_source_prepare(GSource *base,
							 int *timeout)
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
wayland_event_source_dispatch(GSource *base,
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

static GSourceFuncs wayland_event_source_funcs = {
	wayland_event_source_prepare,
	NULL,
	wayland_event_source_dispatch,
	NULL
};


static GSource *
wayland_event_source_new(struct wl_display *display)
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
after_paint(ClutterStage *stage,
			gpointer data)
{
	/* TF; */
	struct surface *surface;
	wl_list_for_each(surface, &surfaces, link) {
		if (surface->frame_callback) {
			/* TF; */
			wl_callback_send_done (surface->frame_callback, g_get_monotonic_time()/1000);
			wl_resource_destroy (surface->frame_callback);
			surface->frame_callback = NULL;
		}
	}

}


int
main (int argc, char **argv) {

	printf("argc: %d\n", argc);
	printf("argv: %s\n", argv[0]);

	wl_list_init(&clients);
	wl_list_init(&surfaces);
	display = wl_display_create ();
	/* clutter_wayland_set_compositor_display(display); */
	clutter_set_windowing_backend("x11");

	GSource *wayland_event_source;

	// wayland event loop
	wayland_event_source = wayland_event_source_new (display);
	g_source_attach(wayland_event_source, NULL);


	wl_global_create(display, &wl_compositor_interface, 3, NULL, &compositor_bind);
	wl_global_create(display, &wl_shell_interface, 1, NULL, &shell_bind);
	wl_global_create(display, &xdg_shell_interface, 1, NULL, &xdg_shell_bind);
	wl_global_create(display, &wl_seat_interface, 1, NULL, &seat_bind);
	wl_display_init_shm(display);

    xdg_bind_init();

	// stage
	// clutter
	// gjs


	clutter_init(&argc, &argv);

	wl_display_add_socket_auto(display);

	stage = clutter_stage_new ();
	clutter_stage_set_user_resizable(stage, TRUE);
	clutter_actor_show(stage);

	scroll = clutter_scroll_actor_new();
	clutter_actor_add_child(stage,scroll);
	clutter_actor_set_layout_manager(scroll, clutter_box_layout_new());


	ClutterDeviceManager *device_manager =
		clutter_device_manager_get_default();

	GSList *devices =
		clutter_device_manager_list_devices(device_manager);

	ClutterInputDevice *input =
		clutter_device_manager_get_core_device(device_manager, CLUTTER_POINTER_DEVICE);
	ClutterInputDevice *keyboard =
		clutter_device_manager_get_core_device(device_manager, CLUTTER_KEYBOARD_DEVICE);

	struct wl_seat *pointer = clutter_wayland_input_device_get_wl_seat(input);
	struct wl_seat *wl_keyboard = clutter_wayland_input_device_get_wl_seat(keyboard);

	if(pointer == NULL)
		printf("pointer is null\n");
	if (wl_keyboard == NULL)
		printf("wl_keyboard is null\n");

	if (pointer == wl_keyboard) {
		printf("pointer and keyboard on same seat\n");
	} else {
		printf("pointer and keyboard is not on the same seat\n");
	}

	guint length = g_list_length(devices);
	printf("length: %d\n", length);
	for (int i=0; i < length; i++) {
		GSList *list = g_list_nth(devices, i);
		ClutterInputDevice *device = list->data;
		printf("device: %s\n", clutter_input_device_get_device_name(device));
	}

	GMainLoop *loop = g_main_loop_new (NULL, FALSE);

	g_signal_connect(stage, "after-paint", G_CALLBACK(after_paint), loop);

	g_main_loop_run (loop);

	wl_display_destroy (display);
	return 0;
}
