
#include <wayland-server.h>
#include "xdg-shell-unstable-v6-server-protocol.h"
#include "compositor.h"

// toplevel

void
zxdg_toplevel_destroy(struct wl_client *client,
                      struct wl_resource *resource) {
}
void
zxdg_toplevel_set_parent(struct wl_client *client,
                         struct wl_resource *resource,
                         struct wl_resource *parent) {
}
void
zxdg_toplevel_set_title(struct wl_client *client,
                        struct wl_resource *resource,
                        const char *title) {
}
void
zxdg_toplevel_set_app_id(struct wl_client *client,
                         struct wl_resource *resource,
                         const char *app_id) {
}
void
zxdg_toplevel_show_window_menu(struct wl_client *client,
                               struct wl_resource *resource,
                               struct wl_resource *seat,
                               uint32_t serial,
                               int32_t x,
                               int32_t y) {
}
void
zxdg_toplevel_move(struct wl_client *client,
                   struct wl_resource *resource,
                   struct wl_resource *seat,
                   uint32_t serial) {
}
void
zxdg_toplevel_resize(struct wl_client *client,
                     struct wl_resource *resource,
                     struct wl_resource *seat,
                     uint32_t serial,
                     uint32_t edges) {
}
void
zxdg_toplevel_set_max_size(struct wl_client *client,
                           struct wl_resource *resource,
                           int32_t width,
                           int32_t height) {
}
void
zxdg_toplevel_set_min_size(struct wl_client *client,
                           struct wl_resource *resource,
                           int32_t width,
                           int32_t height) {
}
void
zxdg_toplevel_set_maximized(struct wl_client *client,
                            struct wl_resource *resource) {
}
void
zxdg_toplevel_unset_maximized(struct wl_client *client,
                              struct wl_resource *resource) {
}
void
zxdg_toplevel_set_fullscreen(struct wl_client *client,
                             struct wl_resource *resource,
                             struct wl_resource *output) {
}
void
zxdg_toplevel_unset_fullscreen(struct wl_client *client,
                               struct wl_resource *resource) {
}
void
zxdg_toplevel_set_minimized(struct wl_client *client,
                            struct wl_resource *resource) {
}

extern struct zxdg_toplevel_v6_interface zxdg_toplevel_v6_interface_impl = {
    .destroy = zxdg_toplevel_destroy,
    .set_parent = zxdg_toplevel_set_parent,
    .set_title = zxdg_toplevel_set_title,
    .set_app_id = zxdg_toplevel_set_app_id,
    .show_window_menu = zxdg_toplevel_show_window_menu,
    .move = zxdg_toplevel_move,
    .resize = zxdg_toplevel_resize,
    .set_max_size = zxdg_toplevel_set_max_size,
    .set_min_size = zxdg_toplevel_set_min_size,
    .set_maximized = zxdg_toplevel_set_maximized,
    .unset_maximized = zxdg_toplevel_unset_maximized,
    .set_fullscreen = zxdg_toplevel_set_fullscreen,
    .unset_fullscreen = zxdg_toplevel_unset_fullscreen,
    .set_minimized = zxdg_toplevel_set_minimized
};


// Positioner

/**
 * destroy the xdg_positioner object
 *
 * Notify the compositor that the xdg_positioner will no longer
 * be used.
 */
void
zxdg_positioner_destroy(struct wl_client *client,
                        struct wl_resource *resource) {

}

/**
 * set the size of the to-be positioned rectangle
 *
 * Set the size of the surface that is to be positioned with the
 * positioner object. The size is in surface-local coordinates and
 * corresponds to the window geometry. See
 * xdg_surface.set_window_geometry.
 *
 * If a zero or negative size is set the invalid_input error is
 * raised.
 * @param width width of positioned rectangle
 * @param height height of positioned rectangle
 */
void
zxdg_positioner_set_size(struct wl_client *client,
                         struct wl_resource *resource,
                         int32_t width,
                         int32_t height) {

}

/**
 * set the anchor rectangle within the parent surface
 *
 * Specify the anchor rectangle within the parent surface that
 * the child surface will be placed relative to. The rectangle is
 * relative to the window geometry as defined by
 * xdg_surface.set_window_geometry of the parent surface. The
 * rectangle must be at least 1x1 large.
 *
 * When the xdg_positioner object is used to position a child
 * surface, the anchor rectangle may not extend outside the window
 * geometry of the positioned child's parent surface.
 *
 * If a zero or negative size is set the invalid_input error is
 * raised.
 * @param x x position of anchor rectangle
 * @param y y position of anchor rectangle
 * @param width width of anchor rectangle
 * @param height height of anchor rectangle
 */
void
zxdg_positioner_set_anchor_rect(struct wl_client *client,
                                struct wl_resource *resource,
                                int32_t x,
                                int32_t y,
                                int32_t width,
                                int32_t height) {

}

/**
 * set anchor rectangle anchor edges
 *
 * Defines a set of edges for the anchor rectangle. These are
 * used to derive an anchor point that the child surface will be
 * positioned relative to. If two orthogonal edges are specified
 * (e.g. 'top' and 'left'), then the anchor point will be the
 * intersection of the edges (e.g. the top left position of the
 * rectangle); otherwise, the derived anchor point will be centered
 * on the specified edge, or in the center of the anchor rectangle
 * if no edge is specified.
 *
 * If two parallel anchor edges are specified (e.g. 'left' and
 * 'right'), the invalid_input error is raised.
 * @param anchor bit mask of anchor edges
 */
void
zxdg_positioner_set_anchor(struct wl_client *client,
                           struct wl_resource *resource,
                           uint32_t anchor) {

}

/**
 * set child surface gravity
 *
 * Defines in what direction a surface should be positioned,
 * relative to the anchor point of the parent surface. If two
 * orthogonal gravities are specified (e.g. 'bottom' and 'right'),
 * then the child surface will be placed in the specified
 * direction; otherwise, the child surface will be centered over
 * the anchor point on any axis that had no gravity specified.
 *
 * If two parallel gravities are specified (e.g. 'left' and
 * 'right'), the invalid_input error is raised.
 * @param gravity bit mask of gravity directions
 */
void
zxdg_positioner_set_gravity(struct wl_client *client,
                            struct wl_resource *resource,
                            uint32_t gravity) {

}

/**
 * set the adjustment to be done when constrained
 *
 * Specify how the window should be positioned if the originally
 * intended position caused the surface to be constrained, meaning
 * at least partially outside positioning boundaries set by the
 * compositor. The adjustment is set by constructing a bitmask
 * describing the adjustment to be made when the surface is
 * constrained on that axis.
 *
 * If no bit for one axis is set, the compositor will assume that
 * the child surface should not change its position on that axis
 * when constrained.
 *
 * If more than one bit for one axis is set, the order of how
 * adjustments are applied is specified in the corresponding
 * adjustment descriptions.
 *
 * The default adjustment is none.
 * @param constraint_adjustment bit mask of constraint adjustments
 */
void
zxdg_positioner_set_constraint_adjustment(struct wl_client *client,
                                          struct wl_resource *resource,
                                          uint32_t constraint_adjustment) {

}

/**
 * set surface position offset
 *
 * Specify the surface position offset relative to the position
 * of the anchor on the anchor rectangle and the anchor on the
 * surface. For example if the anchor of the anchor rectangle is at
 * (x, y), the surface has the gravity bottom|right, and the offset
 * is (ox, oy), the calculated surface position will be (x + ox, y
 * + oy). The offset position of the surface is the one used for
 * constraint testing. See set_constraint_adjustment.
 *
 * An example use case is placing a popup menu on top of a user
 * interface element, while aligning the user interface element of
 * the parent surface with some user interface element placed
 * somewhere in the popup surface.
 * @param x surface position x offset
 * @param y surface position y offset
 */
void
zxdg_positioner_set_offset(struct wl_client *client,
                           struct wl_resource *resource,
                           int32_t x,
                           int32_t y) {

}

extern struct zxdg_positioner_v6_interface zxdg_positioner_v6_interface_impl = {
    .destroy = zxdg_positioner_destroy,
    .set_size = zxdg_positioner_set_size,
    .set_anchor_rect = zxdg_positioner_set_anchor_rect,
    .set_anchor = zxdg_positioner_set_anchor,
    .set_gravity = zxdg_positioner_set_gravity,
    .set_constraint_adjustment = zxdg_positioner_set_constraint_adjustment,
    .set_offset = zxdg_positioner_set_offset
};


/**
 * assign the xdg_toplevel surface role
 *
 * This creates an xdg_toplevel object for the given xdg_surface
 * and gives the associated wl_surface the xdg_toplevel role.
 *
 * See the documentation of xdg_toplevel for more details about
 * what an xdg_toplevel is and how it is used.
 */
void
zxdg_surface_get_toplevel_impl(struct wl_client *client,
                               struct wl_resource *resource,
                               uint32_t id) {
    struct wl_resource *res =
        wl_resource_create (client, &zxdg_toplevel_v6_interface, 1, id);
    wl_resource_set_implementation (res,
                                    &zxdg_toplevel_v6_interface_impl,
                                    id,
                                    NULL);
}

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
void
zxdg_surface_ack_configure_impl(struct wl_client *client,
                                struct wl_resource *resource,
                                uint32_t serial) {
    printf("ack configuer request");
}

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
void
zxdg_surface_set_window_geometry_impl(struct wl_client *client,
                                      struct wl_resource *resource,
                                      int32_t x,
                                      int32_t y,
                                      int32_t width,
                                      int32_t height) {

}

/**
 * assign the xdg_popup surface role
 *
 * This creates an xdg_popup object for the given xdg_surface and
 * gives the associated wl_surface the xdg_popup role.
 *
 * See the documentation of xdg_popup for more details about what
 * an xdg_popup is and how it is used.
 */
void
zxdg_surface_get_popup_impl(struct wl_client *client,
                            struct wl_resource *resource,
                            uint32_t id,
                            struct wl_resource *parent,
                            struct wl_resource *positioner) {

}

/**
 * destroy the xdg_surface
 *
 * Destroy the xdg_surface object. An xdg_surface must only be
 * destroyed after its role object has been destroyed.
 */
void
zxdg_surface_destroy_impl(struct wl_client *client,
                          struct wl_resource *resource) {

}

static struct zxdg_surface_v6_interface zxdg_surface_v6_interface_impl = {
    .destroy = zxdg_surface_destroy_impl,
    .get_toplevel = zxdg_surface_get_toplevel_impl,
    .get_popup = zxdg_surface_get_popup_impl,
    .set_window_geometry = zxdg_surface_set_window_geometry_impl,
    .ack_configure = zxdg_surface_ack_configure_impl

};

// shell

/**
 * destroy xdg_shell
 *
 * Destroy this xdg_shell object.
 *
 * Destroying a bound xdg_shell object while there are surfaces
 * still alive created by this xdg_shell object instance is illegal
 * and will result in a protocol error.
 */
void
zxdg_shell_destroy_impl(struct wl_client *client,
                        struct wl_resource *resource) {
}
/**
 * create a positioner object
 *
 * Create a positioner object. A positioner object is used to
 * position surfaces relative to some parent surface. See the
 * interface description and xdg_surface.get_popup for details.
 */
void
zxdg_shell_create_positioner_impl(struct wl_client *client,
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
void
zxdg_shell_get_xdg_surface_impl(struct wl_client *client,
                                struct wl_resource *resource,
                                uint32_t id,
                                struct wl_resource *surface) {

    struct wl_resource *res =
        wl_resource_create (client, &zxdg_surface_v6_interface, 1, id);
    wl_resource_set_implementation (res,
                                    &zxdg_surface_v6_interface_impl,
                                    surface,
                                    NULL);

    uint32_t serial = wl_display_next_serial(display);
    zxdg_surface_v6_send_configure(res, serial);
}
/**
 * respond to a ping event
 *
 * A client must respond to a ping event with a pong request or
 * the client may be deemed unresponsive. See xdg_shell.ping.
 * @param serial serial of the ping event
 */
void
zxdg_shell_pong_impl(struct wl_client *client,
                     struct wl_resource *resource,
                     uint32_t serial) {
}

static struct zxdg_shell_v6_interface zxdg_shell_v6_interface_impl = {
    .destroy = zxdg_shell_destroy_impl,
    .create_positioner = zxdg_shell_create_positioner_impl,
    .get_xdg_surface = zxdg_shell_get_xdg_surface_impl,
    .pong = zxdg_shell_pong_impl
};


void
zxdg_popup_destroy(struct wl_client *client,
                   struct wl_resource *resource) {
}
void
zxdg_popup_grab(struct wl_client *client,
                struct wl_resource *resource,
                struct wl_resource *seat,
                uint32_t serial) {
}

extern struct zxdg_popup_v6_interface zxdg_popup_v6_interface_impl = {
    .destroy = zxdg_popup_destroy,
    .grab = zxdg_popup_grab
};


extern void
zxdg_toplevel_bind(struct wl_client *client,
                   void *data, uint32_t version,
                   uint32_t id) {
    struct wl_resource *resource = wl_resource_create (client, &zxdg_toplevel_v6_interface, 1, id);
    wl_resource_set_implementation (resource, &zxdg_toplevel_v6_interface_impl, NULL, NULL);
}

extern void
zxdg_surface_bind(struct wl_client *client,
                  void *data,
                  uint32_t version,
                  uint32_t id) {
    struct wl_resource *resource = wl_resource_create (client, &zxdg_surface_v6_interface, 1, id);
    wl_resource_set_implementation (resource, &zxdg_surface_v6_interface_impl, NULL, NULL);
}

extern void
zxdg_shell_bind(struct wl_client *client,
                void *data,
                uint32_t version,
                uint32_t id) {
    struct wl_resource *resource = wl_resource_create (client, &zxdg_shell_v6_interface, 1, id);
    wl_resource_set_implementation (resource, &zxdg_shell_v6_interface_impl, NULL, NULL);
}
