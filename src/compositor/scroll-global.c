#include "scroll-global.h"

#include <stdarg.h>

struct _ScrollGlobal {
    GObject parent;

    ClutterStage *stage;
};

G_DEFINE_TYPE(ScrollGlobal, scroll_global, G_TYPE_OBJECT)

static ScrollGlobal *global = NULL;


/**
 * scroll_global_get:
 *
 * Gets the singleton global object
 *
 * Return value: (transfer none): the singleton global object
 */
ScrollGlobal *
scroll_global_get(void) {
    if(global == NULL) {
        global = SCROLL_GLOBAL(g_object_newv(SCROLL_TYPE_GLOBAL, 0, NULL));
    }
    return global;
}

/**
 * scroll_global_get_stage:
 *
 * Return value: (transfer none): The default #ClutterStage
 */
ClutterStage *
scroll_global_get_stage (ScrollGlobal  *global)
{
    return global->stage;
}

static void
scroll_global_class_init (ScrollGlobalClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
}

static void
scroll_global_init(ScrollGlobal *self) {
    self->stage = clutter_stage_get_default();
}

