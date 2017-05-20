
settings = new Gio.Settings({ schema_id: "org.gnome.desktop.wm.keybindings"});
settings.set_strv("cycle-windows", ["<alt>period"])
settings.set_strv("cycle-windows-backward", ["<alt>comma"])
settings.set_strv("switch-windows", [])
Meta.keybindings_set_custom_handler("cycle-windows", next);
Meta.keybindings_set_custom_handler("cycle-windows-backward", previous);
