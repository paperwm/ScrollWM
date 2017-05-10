#ifndef SHELL_GLOBAL_H
#define SHELL_GLOBAL_H

#include <clutter/clutter.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define SHELL_TYPE_GLOBAL (shell_global_get_type ())

G_DECLARE_FINAL_TYPE (ShellGlobal, shell_global, SHELL, GLOBAL, GObject)

ShellGlobal  *shell_global_get          (void);
ClutterStage  *shell_global_get_stage    (ShellGlobal *global);

G_END_DECLS

#endif /* SHELL_GLOBAL_H */
