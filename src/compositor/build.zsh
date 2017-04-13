#!/usr/bin/env nix-shell
#! nix-shell -i bash --pure

function pkgs() {
    pkg-config --cflags --libs clutter-egl-1.0
}

gcc -o wayland-compositor wayland-compositor.c xdg-shell-*protocol.c \
   $(pkgs) 


# pkgconfig xorg.libX11 xorg.libxcb libxkbcommon mesa wayland 
