with import ~/nixpkgs {};

stdenv.mkDerivation rec {
  version = "0.x";
  name = "scroll-wm";

  src = ./.;

  buildInputs =
    with xorg; [ libX11 mesa libXext libXfixes libXdamage libXcomposite libXi ] ++
    [cogl gobjectIntrospection atk pango json_glib cairo pkgconfig gnome3.mutter xwayland wayland wayland-protocols ];

  installFlags = "PREFIX=\${out}";

}
