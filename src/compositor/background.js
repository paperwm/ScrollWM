const Clutter = imports.gi.Clutter;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Cogl = imports.gi.Cogl;

let stage = Clutter.Stage.get_default();

let pc = Clutter.color_from_string("black");
stage.set_color(pc[1]);

let image = new Clutter.Image();
let pixbuf = GdkPixbuf.Pixbuf.new_from_file("background.png");
image.set_data(pixbuf.get_pixels() ,
               pixbuf.get_has_alpha() ? Cogl.PixelFormat.RGBA_8888
               : Cogl.PixelFormat.RGB_888,
               pixbuf.get_width() ,
               pixbuf.get_height() ,
               pixbuf.get_rowstride());
stage.set_content(image);



