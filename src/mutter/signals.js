const shell = imports.gi.Shell
const global = shell.Global.get()
const wm = global.get_shell_wm()

wm.connect("map", (wm, actor) => {
    mapWindow(wm, actor)
    wm.completed_map(actor);
})

wm.connect("destroy", (wm, actor) => {
    actor.connect("destroy", (actor) => {
        actor.get_parent().destroy();
    })
    wm.completed_destroy(actor);
})

wm.connect("minimize", (wm, actor) => {
    wm.completed_minimize(actor);
})

wm.connect("unminimize", (wm, actor) => {
    wm.completed_unminimize(actor);
})

wm.connect("size_change", (wm, actor) => {
    wm.completed_size_change(actor);
})

wm.connect('filter-keybinding', (wm, binding) => {
    let name = binding.get_name();
    print("binding: " + name);
    bind = binding;
    return false;
});

focus = (win, data) => {
    actor = win.get_compositor_private();
    print(actor.toString());
    if (lastFocus) {

    }
    let decoration = actor.get_parent();
    decoration.set_background_color(cyan);
    let overlap = 20;
    if (decoration === scroll.get_first_child() ||
        decoration === scroll.get_last_child()) {
        overlap = 0;
    }
    let absPosition = decoration
        .apply_relative_transform_to_point(scroll,
                                           new Clutter.Vertex({x: 0, y: 0, z: 0}));
    if (absPosition.x < 0) {
        print("scroll left: " + (decoration.x));
        scroll.scroll_to_point(new Clutter.Point({ x: decoration.x - overlap,
                                                   y: decoration.y }));
    } else if (absPosition.x + decoration.width + 20 > 0 + stage.width ) {
        print("scroll right: " + (decoration.x + scroll.width));
        scroll.scroll_to_point(new Clutter.Point({
            x: decoration.x + decoration.width - scroll.width + overlap,
            y: decoration.y }));
    }
};

focusWrapper = (actor, data) => {
    focus(actor, data);
};

leave = (actor) => {
    let decoration = actor.get_parent();
    decoration.set_background_color(grey);
}

leaveWrapper = (actor) => {
    leave(actor);
};

