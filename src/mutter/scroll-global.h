#ifndef SCROLL_GLOBAL_H
#define SCROLL_GLOBAL_H

#include <clutter/clutter.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define SCROLL_TYPE_GLOBAL (scroll_global_get_type ())

G_DECLARE_FINAL_TYPE (ScrollGlobal, scroll_global, SCROLL, GLOBAL, GObject)

ScrollGlobal  *scroll_global_get          (void);
ClutterStage  *scroll_global_get_stage    (ScrollGlobal *global);

G_END_DECLS

#endif /* SCROLL_GLOBAL_H */
