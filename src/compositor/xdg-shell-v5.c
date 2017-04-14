#include "compositor.h"


static int pointer_x, pointer_y;
static struct surface *moving_surface = NULL;
static struct surface *active_surface = NULL;

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

void
xdg_v5_bind_init() {
    wl_global_create(display, &xdg_shell_interface, 1, NULL, &xdg_shell_bind);

}
