with import ~/nixpkgs {};

let
  # Clutter with proper wayland support
  # clutter = callPackage ../../clutter.nix {  };
in

stdenv.mkDerivation rec {
  version = "0.x";
  name = "scroll-wm";

  src = ./.;

  MUTTER_GI_PATH="${gnome3.mutter}/lib/mutter";

  preConfigure = ''
  shell_dir=${wayland-protocols}/share/wayland-protocols/unstable/xdg-shell
  ${wayland}/bin/wayland-scanner server-header < $shell_dir/xdg-shell-unstable-v5.xml \
    > xdg-shell-unstable-v5-server-protocol.h
  ${wayland}/bin/wayland-scanner code < $shell_dir/xdg-shell-unstable-v5.xml \
    > xdg-shell-unstable-v5-server-protocol.c

  ${wayland}/bin/wayland-scanner server-header < $shell_dir/xdg-shell-unstable-v6.xml \
    > xdg-shell-unstable-v6-server-protocol.h
  ${wayland}/bin/wayland-scanner code < $shell_dir/xdg-shell-unstable-v6.xml \
    > xdg-shell-unstable-v6-server-protocol.c
  '';

  buildInputs =
    with xorg; [ libX11 mesa libXext libXfixes libXdamage libXcomposite libXi ] ++
    [cogl gnome3.gjs gobjectIntrospection atk pango json_glib cairo gnome3.mutter pkgconfig xwayland wayland wayland-protocols gnome3.gsettings_desktop_schemas gnome3.gtk3 ];

  installFlags = "PREFIX=\${out}";

}
