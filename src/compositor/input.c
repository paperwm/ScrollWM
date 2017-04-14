
#include "compositor.h"

#include <xkbcommon/xkbcommon.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>

void
get_keymap(int *fd, int *size) {
    struct xkb_context *context;
    struct xkb_keymap *keymap;
    context = xkb_context_new(0);
    keymap = xkb_keymap_new_from_names(context, NULL, 0);
    char *string = xkb_keymap_get_as_string (keymap, XKB_KEYMAP_FORMAT_TEXT_V1);

    *size = strlen (string) + 1;
    char *xdg_runtime_dir = getenv ("XDG_RUNTIME_DIR");
    *fd = open (xdg_runtime_dir, __O_TMPFILE|O_RDWR|O_EXCL, 0600);
    ftruncate (*fd, *size);
    char *map = mmap (NULL, *size, PROT_READ|PROT_WRITE, MAP_SHARED, *fd, 0);
    strcpy (map, string);
    munmap (map, *size);
    free (string);
}

static gboolean
forward_key_event(ClutterActor *actor, ClutterKeyEvent *event, gpointer data, int state) {
    struct surface *surface = data;
    struct client *client = surface->client;

    if(client->keyboard) {
        uint32_t serial = wl_display_next_serial(display);
        
        // -8 is necessary to align keycodes for some reason (see comment for
        // -WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
        wl_keyboard_send_key(client->keyboard, serial, event->time, event->hardware_keycode-8, state);
    }
    
}

gboolean
key_press_event(ClutterActor *actor,
            ClutterKeyEvent *event,
            gpointer data) {
    forward_key_event(actor, event, data, 1);
}

gboolean
key_release_event(ClutterActor *actor,
                  ClutterKeyEvent *event,
                  gpointer data) {
    forward_key_event(actor, event, data, 0);
}

gboolean
forward_button_event(ClutterActor *actor,
                     ClutterButtonEvent *event,
                     gpointer data,
                     uint32_t state) {
    struct surface *surface = data;
    uint32_t serial = wl_display_next_serial(display);
    wl_pointer_send_button(surface->client->pointer,
                           serial,
                           event->time,
                           // For some reason wayland wants button 1 to be 272
                           event->button + 271,
                           state);
}

gboolean
button_press_event(ClutterActor *actor,
                   ClutterButtonEvent *event,
                   gpointer data) {
    forward_button_event(actor, event, data, 1);
}

gboolean
button_release_event(ClutterActor *actor,
                   ClutterButtonEvent *event,
                   gpointer data) {
    forward_button_event(actor, event, data, 0);
}

gboolean
motion_event(ClutterActor *actor,
             ClutterMotionEvent *event,
             gpointer      data) {

    gfloat x; gfloat y;
    clutter_actor_transform_stage_point(actor, event->x, event->y, &x, &y);
    struct surface *surface = data;

    wl_pointer_send_motion(surface->client->pointer,
                           event->time,
                           wl_fixed_from_double(x),
                           wl_fixed_from_double(y));
    return TRUE;
}

gboolean
enter_event(ClutterActor *actor,
            ClutterCrossingEvent *event,
            gpointer data) {

    struct surface *surface = data;
    struct client *client = surface->client;
    printf("enter_event: %s (%.2f, %.2f)\n", clutter_actor_get_name(actor), event->x, event->y);
    gboolean event_consumed = FALSE;
    if(client->pointer) {
        uint32_t serial = wl_display_next_serial(display);
        gfloat x; gfloat y;
        clutter_actor_transform_stage_point(actor, event->x, event->y, &x, &y);
        wl_pointer_send_enter(client->pointer, serial, surface->surface,
                              wl_fixed_from_double(x),
                              wl_fixed_from_double(y));
        g_signal_connect(actor, "motion-event", G_CALLBACK(motion_event), surface);
        g_signal_connect(actor, "button-press-event", G_CALLBACK(button_press_event),
                         surface);
        g_signal_connect(actor, "button-release-event", G_CALLBACK(button_release_event),
                         surface);
        event_consumed = TRUE;

    }
    if(client->keyboard) {
        struct wl_array empty;
        wl_array_init(&empty);
        wl_keyboard_send_modifiers(client->keyboard,
                                   wl_display_next_serial(display),
                                   0, 0,
                                   0, 0);
        wl_keyboard_send_enter(client->keyboard, wl_display_next_serial(display), surface->surface, &empty);

        struct wl_array states;
        wl_array_init(&states);
        uint32_t *s = wl_array_add(&states, sizeof(uint32_t));
        *s = ZXDG_TOPLEVEL_V6_STATE_ACTIVATED;
        s = wl_array_add(&states, sizeof(uint32_t));
        *s = ZXDG_TOPLEVEL_V6_STATE_MAXIMIZED;

        zxdg_toplevel_v6_send_configure(surface->xdg_toplevel_surface, 0, 0, &states);
        clutter_stage_set_key_focus(stage, actor);
        event_consumed = TRUE;

        ClutterPoint point;
        clutter_actor_get_position(actor, &(point.x), &(point.y));
        point.x -= 20;
        clutter_scroll_actor_scroll_to_point (scroll, &point);
    }
    return event_consumed;
}

gboolean
leave_event(ClutterActor *actor,
            ClutterEvent *event,
            gpointer      data) {

    struct surface *surface = data;
    struct client *client = surface->client;
    gboolean event_consumed = FALSE;
    if(client->pointer) {
        uint32_t serial = wl_display_next_serial(display);
        wl_pointer_send_leave(client->pointer, serial, surface->surface);
        g_signal_handlers_disconnect_by_func(actor, G_CALLBACK(motion_event), surface);
        g_signal_handlers_disconnect_by_func(actor, G_CALLBACK(button_press_event), surface);
        g_signal_handlers_disconnect_by_func(actor, G_CALLBACK(button_release_event), surface);
        event_consumed = TRUE;
    }
    if(client->keyboard) {
        wl_keyboard_send_leave(client->keyboard,
                               wl_display_next_serial(display),
                               surface->surface);

        struct wl_array states;
        wl_array_init(&states);
        uint32_t *s = wl_array_add(&states, sizeof(uint32_t));
        *s = ZXDG_TOPLEVEL_V6_STATE_MAXIMIZED;

        zxdg_toplevel_v6_send_configure(surface->xdg_toplevel_surface, 0, 0, &states);
        clutter_stage_set_key_focus(stage, NULL);
        event_consumed = TRUE;
    }
    return event_consumed;

}
