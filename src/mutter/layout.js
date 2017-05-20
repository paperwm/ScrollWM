
Clutter = imports.gi.Clutter;
Meta = imports.gi.Meta

GObject = imports.gi.GObject;

Glib = imports.gi.GLib

shell = imports.gi.Shell
global = shell.Global.get()
stage = global.get_stage()

wm = global.get_shell_wm()

focus = display.focus_window
//: [object instance proxy GType:MetaWindowWayland jsobj@0x7ff315474760 native@0xabe500]

next = () => {
    let focus = display.focus_window;
    let actor = focus.get_compositor_private();
    let decoration = actor.get_parent();
    let next = decoration.get_next_sibling();
    next && next.first_child.meta_window.activate(Glib.get_monotonic_time()/1000);
}

previous = () => {
    let focus = display.focus_window;
    let actor = focus.get_compositor_private();
    let decoration = actor.get_parent();
    let prev = decoration.get_previous_sibling();
    prev && prev.first_child.meta_window.activate(Glib.get_monotonic_time()/1000);
}

is_decoration = (actor) => {
    if(!actor)
        return false;
    let children = actor.get_children();
    if(children.length === 0)
        return false;
    return GObject.type_name(children[0]) === "ClutterWaylandSurface";
}

deep_clone = (actor) => {
    let clone;
    if(is_decoration(actor)) {
        clone = new Clutter.Clone({source: actor});
        clone.set_position(actor.x, actor.y);
    } else {
        // clone = new actor.constructor();
        clone = new Clutter.Actor();
        clone.set_position(actor.x, actor.y);
        clone.set_size(actor.width, actor.height);

        actor.get_children().forEach((child) => {
            clone.add_child(deep_clone(child));
        });
    }

    return clone;
}

create_minimap = (workspace) => {
    let minimap = deep_clone(workspace);

    let scale = 0.2;

    minimap.set_size(scroll.width/scale, scroll.height/scale);
    minimap.set_scale(scale, scale);

    return minimap;
}

() => {
    minimap = create_minimap(scroll)
    stage.add_child(minimap)
    minimap.destroy()

    scroll = imports.gi.Scroll
    global = scroll.Global.get()
    stage = global.get_stage()

    stage.connect("new-window", (stage, actor) => {
        print("show");
        actor.show();
    })


    stage.first_child.first_child
//: [object instance proxy GType:MetaWindowActor jsobj@0x7f4d34a70d90 native@0x216c800]
    //: ~
    stage.first_child.first_child.meta_window
    //: t
    stage.first_child.first_child.first_child
    //: [object instance proxy GType:MetaSurfaceActorWayland jsobj@0x7f84431b4f10 native@0x8c8b40]
    stage.first_child.first_child.first_child.first_child
    //: [object instance proxy GType:MetaShapedTexture jsobj@0x7f84431cd880 native@0x8bad10]

    scroll.first_child.first_child.width = scroll.first_child.first_child.first_child.width

    scroll.first_child.first_child.meta_window.move_resize_frame(true, 0, 0, 300, 400)

    scroll.first_child.first_child.width = scroll.first_child.first_child.first_child.width

    overlap = 20;
    decoration = scroll.get_children()[0];
//: [object instance proxy GIName:Clutter.Actor jsobj@0x7f61d0635fd0 native@0xfa5460]
    scroll.scroll_to_point(new Clutter.Point({ x: decoration.x - overlap,
                                               y: decoration.y }));

    stage.first_child.first_child.height
//: 491
    stage.first_child.first_child.first_child.height
//: 251


    scroll.show()

    mw = scroll.first_child.first_child.get_meta_window()
//: [object instance proxy GType:MetaWindowWayland jsobj@0x7efe9c4c01c0 native@0x2c1c150]

    mw.move_resize_frame(true, 0, 0, 400, 500)
//: t
    scroll.first_child
//: t



    scroll.first_child.first_child.first_child
//: [object instance proxy GType:MetaSurfaceActorWayland jsobj@0x7fe6858740a0 native@0x264cb40]

}

workspace = new Clutter.Actor();

stage.add_child(workspace);

red = Clutter.Color.from_string("red")[1];
grey = Clutter.Color.from_string("grey")[1];
cyan = Clutter.Color.from_string("cyan")[1];
blue = Clutter.Color.from_string("blue")[1];
workspace.set_background_color(red)


scroll = new Clutter.ScrollActor();
scroll.set_easing_duration(250);
stage.connect("activate", (stage) => {
    workspace.width = stage.width;
    workspace.height = stage.height;
    scroll.set_position(10,10);
    scroll.width = workspace.width - 20;
    scroll.height = workspace.height - 20;
})

scroll.set_scroll_mode(Clutter.ScrollMode.HORIZONTALLY);

workspace.add_child(scroll);

boxlayout = new Clutter.BoxLayout();

scroll.set_layout_manager(boxlayout);
//: t

scroll.set_background_color(blue);



margin = new Clutter.Margin({
    left: 4,
    right: 4,
    top: 4,
    bottom: 4
});


() => {
    actor.grab_key_focus()
    actor = scroll.get_children()[2].get_children()[0]
    [_, vertex.x, absY] = stage.transform_stage_point(actor.x, actor.y)
    [_, vertex.x, absY, _] = scroll.apply_relative_transform_to_point(stage, new Clutter.Vertex({
        x: actor.x, y: actor.y, z: 0}));

    frame = scroll.last_child.first_child.meta_window.get_buffer_rect()
    scroll.last_child.first_child.x
//: 43

    frame.y
//: -32
}


lastFocus = true;
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

group = stage.first_child;
mapWindow = (wm, actor) => {
    print(actor.toString());
    group.remove_child(actor);
    let decoration = new Clutter.Actor();
    decoration.set_background_color(grey);
    actor.set_margin(margin)
    decoration.add_child(actor);
    scroll.add_child(decoration);
    actor.meta_window.move_resize_frame(false, 0, 0, 500, 700)
    actor.meta_window.connect("focus", focusWrapper)
    actor.first_child.first_child.connect("button-press-event", focusWrapper);
    decoration.show();
    actor.show();
}

scroll.get_children()[0]
//: [object instance proxy GIName:Clutter.Actor jsobj@0x7f4145074280 native@0xe41110]

scroll.get_children().forEach((decoration)=> {
    let actor = decoration.get_children()[0];
    actor.set_size(300,700);
})
