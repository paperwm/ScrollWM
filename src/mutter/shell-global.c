#include "shell-global.h"
#include "compositor.h"

#include <stdarg.h>

struct _ShellGlobal {
    GObject parent;

    MetaScreen *screen;
    MetaPlugin *plugin;
    ShellWM *wm;
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


/**
 * shell_global_get_plugin:
 *
 * Return value: (transfer none):
 */
MetaPlugin *
shell_global_get_plugin(ShellGlobal *global) {
    return global->plugin;
}

/**
 * shell_global_get_shell_wm:
 *
 * Return value: (transfer none):
 */
ShellWM *
shell_global_get_shell_wm(ShellGlobal *global) {
    return global->wm;
}

static void
shell_global_class_init (ShellGlobalClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
}

static void
shell_global_init(ShellGlobal *self) {
}

void
_shell_global_set_plugin(MetaPlugin *shell_plugin) {
    ShellGlobal *global = shell_global_get();
    global->wm = shell_wm_new(shell_plugin);
    global->plugin = shell_plugin;
    global->screen = meta_plugin_get_screen(shell_plugin);
    global->stage = meta_get_stage_for_screen(global->screen);
}

