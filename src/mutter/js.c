#include "js.h"

#include <girepository.h>

GjsContext *js_context;

#define AS_STR(ARG) #ARG
#define MACRO_VALUE_AS_STR(MACRONAME) AS_STR(MACRONAME)

void
js_init() {
    // Use a pure typelib path from build.
    // Having mulitiple clutter versions in the path cause problems for instance.
    setenv("GI_TYPELIB_PATH", MACRO_VALUE_AS_STR(GI_TYPELIB_PATH), 1);

    printf("GI_TYPELIB_PATH: %s\n", getenv("GI_TYPELIB_PATH"));

    js_context = g_object_new (GJS_TYPE_CONTEXT,
                               /* "search-path", search_path, */
                               NULL);

    // Expose a "eval" dbus service
    GError *error = NULL;
    int status;
    gjs_context_eval_file(js_context, "shellDBus.js", &status, &error);
    if(error != NULL)
      printf("js error %s\n", error->message);
}


/**
 * Checks for gobject-introspection arguments. (--introspect-dump)
 * If present, pass control to gobject-introspection (which will exit the program)
 */
void
js_maybe_generate_gir_and_exit(int argc, char **argv) {
    GOptionContext *ctx;
    GError *error = NULL;

    ctx = g_option_context_new (NULL);
    // (g_irepository_get_option_group supplies a callback)
    g_option_context_add_group (ctx, g_irepository_get_option_group ());

    if (!g_option_context_parse (ctx, &argc, &argv, &error)) {
        g_print ("Unexpected error: %s\n", error->message);
        return 1;
    }
}

