#!/usr/bin/env nix-shell
#! nix-shell -i zsh 

function pkgs() {
    pkg-config --cflags --libs wayland-server mutter-clutter-1.0 glib-2.0
}

gcc -o wayland-compositor wayland-compositor.c xdg-shell-*protocol.c \
   $(pkgs) 


# pkgconfig xorg.libX11 xorg.libxcb libxkbcommon mesa wayland 
