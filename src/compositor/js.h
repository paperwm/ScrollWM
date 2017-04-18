#ifndef JS_H
#define JS_H

#include <gjs/gjs.h>
#include <gio/gio.h>

extern void js_init();
extern void js_maybe_generate_gir_and_exit(int argc, char **argv);

extern GjsContext *js_context;

#endif /* JS_H */
