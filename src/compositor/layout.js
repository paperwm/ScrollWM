Clutter = imports.gi.Clutter;

stage = Clutter.Stage.get_default();

workspace = new Clutter.Actor();

stage.add_child(workspace);

red = Clutter.Color.from_string("red")[1];
grey = Clutter.Color.from_string("grey")[1];
cyan = Clutter.Color.from_string("cyan")[1];
blue = Clutter.Color.from_string("blue")[1];
workspace.set_background_color(red)


scroll = new Clutter.ScrollActor();

workspace.add_child(scroll);

boxlayout = new Clutter.BoxLayout();

scroll.set_layout_manager(boxlayout);
//: t

scroll.set_background_color(blue);

workspace.width = stage.width;
workspace.height = stage.height;
scroll.width = workspace.width;
scroll.height = workspace.height;

margin = new Clutter.Margin({
    left: 4,
    right: 4,
    top: 4,
    bottom: 4
});

actor = scroll.get_children()[0]

focus = (actor) => {
    actor.grab_key_focus();
    let decoration = actor.get_parent();
    decoration.set_background_color(cyan);
    [width, height] = stage.get_size();
    vertex = scroll.apply_relative_transform_to_point(stage, new Clutter.Vertex({
        x: decoration.x, y: decoration.y, z: 0}));
    print("absX: " + vertex.x);
    if (vertex.x <= 0) {
        print("scroll right");
        scroll.scroll_to_point(new Clutter.Point({ x: decoration.x, y: decoration.y }));
    } else if (vertex.x + decoration.width > width ) {
        print("scroll left");
        scroll.scroll_to_point(new Clutter.Point({
            x: decoration.x + decoration.width - width,
            y: decoration.y }));
    }
}

leave = (actor) => {
    let decoration = actor.get_parent();
    decoration.set_background_color(grey);
}

stage.connect("new-window", (stage, actor) => {
    print(actor.toString());
    let decoration = new Clutter.Actor();
    decoration.set_background_color(grey);
    actor.set_margin(margin)
    decoration.add_child(actor);
    scroll.add_child(decoration);
    actor.set_size(500,700);
    actor.connect("key-focus-in", focus)
    actor.connect("key-focus-out", leave)
    actor.connect("button-press-event", keyFocus);
})
//: 268

keyFocus = (actor) => {
    actor.grab_key_focus();
}

scroll.get_children().forEach((decoration)=> {
    let actor = decoration.get_children()[0];
    actor.set_size(300,700);
})
