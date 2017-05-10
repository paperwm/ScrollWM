#include "shell-global.h"
#include "compositor.h"

#include <stdarg.h>

struct _ShellGlobal {
    GObject parent;

    ClutterActor *stage;
};

G_DEFINE_TYPE(ShellGlobal, shell_global, G_TYPE_OBJECT)

static ShellGlobal *global = NULL;


/**
 * shell_global_get:
 *
 * Gets the singleton global object
 *
 * Return value: (transfer none): the singleton global object
 */
ShellGlobal *
shell_global_get(void) {
    if(global == NULL) {
        global = SHELL_GLOBAL(g_object_newv(SHELL_TYPE_GLOBAL, 0, NULL));
    }
    return global;
}

/**
 * shell_global_get_stage:
 *
 * Return value: (transfer none): The default #ClutterStage
 */
ClutterStage *
shell_global_get_stage (ShellGlobal  *global)
{
    return global->stage;
}

static void
shell_global_class_init (ShellGlobalClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
}

static void
shell_global_init(ShellGlobal *self) {
    self->stage = stage;
}

