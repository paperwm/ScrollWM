const Clutter = imports.gi.Clutter;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Cogl = imports.gi.Cogl;
const Shell = imports.gi.Shell;

function set_background_image(actor, image_path) {

    let image = new Clutter.Image();

    let pixbuf = GdkPixbuf.Pixbuf.new_from_file(image_path);

    image.set_data(pixbuf.get_pixels() ,
                   pixbuf.get_has_alpha() ? Cogl.PixelFormat.RGBA_8888
                   : Cogl.PixelFormat.RGB_888,
                   pixbuf.get_width() ,
                   pixbuf.get_height() ,
                   pixbuf.get_rowstride());
    actor.set_content(image);
    
}

function set_stage_background_image(path) {
    let global = Shell.Global.get();

    let stage = global.get_stage();

    let pc = Clutter.color_from_string("black");
    stage.set_color(pc[1]);

    set_background_image(stage, path);
}

set_stage_background_image("background.png");
