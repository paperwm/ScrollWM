with import <nixpkgs> {};

let
  # Clutter with proper wayland support
  # clutter = callPackage ../../clutter.nix {  };
in

stdenv.mkDerivation rec {
  version = "0.x";
  name = "scroll-wm";

  src = ./.;

  MUTTER_GI_PATH="${gnome3.mutter}/lib/mutter";

  buildInputs =
    with xorg; [ libX11 mesa libXext libXfixes libXdamage libXcomposite libXi ] ++
    [cogl gnome3.gjs gobjectIntrospection atk pango json_glib cairo gnome3.mutter pkgconfig xwayland wayland wayland-protocols gnome3.gsettings_desktop_schemas gnome3.gtk3 ];

  installFlags = "PREFIX=\${out}";

}
