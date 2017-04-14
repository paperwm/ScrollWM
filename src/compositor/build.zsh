#!/usr/bin/env nix-shell
#! nix-shell -i bash --pure

function pkgs() {
    pkg-config --cflags --libs clutter-wayland-compositor-1.0 clutter-x11-1.0 gdk-pixbuf-2.0 clutter-cogl-1.0
}

gcc -o wayland-compositor *.c $(pkgs)


# pkgconfig xorg.libX11 xorg.libxcb libxkbcommon mesa wayland 
