#ifndef SHELL_GLOBAL_H
#define SHELL_GLOBAL_H

#include <clutter/clutter.h>
#include <glib-object.h>

#include "shell-wm.h"
#include "shell-wm-private.h"

G_BEGIN_DECLS

#define SHELL_TYPE_GLOBAL (shell_global_get_type ())

G_DECLARE_FINAL_TYPE (ShellGlobal, shell_global, SHELL, GLOBAL, GObject)

MetaPlugin   *shell_global_get_plugin(ShellGlobal *global);
ShellWM      *shell_global_get_shell_wm(ShellGlobal *global);
ShellGlobal  *shell_global_get          (void);
ClutterStage *shell_global_get_stage    (ShellGlobal *global);


void _shell_global_set_plugin(MetaPlugin *plugin);
G_END_DECLS


#endif /* SHELL_GLOBAL_H */
