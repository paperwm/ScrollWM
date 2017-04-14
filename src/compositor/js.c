
#include "compositor.h"
#include "js.h"

GjsContext *js_context;

#define AS_STR(ARG) #ARG
#define MACRO_VALUE_AS_STR(MACRONAME) AS_STR(MACRONAME)

void
js_init() {
    // Use a pure typelib path from build.
    // Having mulitiple clutter versions in the path cause problems for instance.
    setenv("GI_TYPELIB_PATH", MACRO_VALUE_AS_STR(GI_TYPELIB_PATH), 1);

    js_context = g_object_new (GJS_TYPE_CONTEXT,
                               /* "search-path", search_path, */
                               NULL);
}
