Clutter = imports.gi.Clutter;
Meta = imports.gi.Meta;

GObject = imports.gi.GObject;

Glib = imports.gi.GLib;

shell = imports.gi.Shell;
global = shell.Global.get();

stage = global.get_stage();
plugin = global.get_plugin();
screen = plugin.get_screen();
display = screen.get_display();
wm = global.get_shell_wm();
