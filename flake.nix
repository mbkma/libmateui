{
  description = "Nix flake to build libmateui (Meson / GTK3)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
    libmateui = pkgs.stdenv.mkDerivation rec {
      pname = "libmateui";
      version = "1.0.0";
      src = ./.;

      # Meson options exposed by meson_options.txt
      mesonFlags = [
        "-Dexamples=true"
        "-Dintrospection=false"
        "-Ddocs=false"
      ];

      nativeBuildInputs = with pkgs; [ meson ninja pkg-config ];

      buildInputs = with pkgs; [
        gtk3
        glib
        xorg.libX11
        xorg.libXScrnSaver
        libSM
        libICE
      ];


      meta = with pkgs.lib; {
        description = "MATE UI library (menus, dialogs, settings, session helpers)";
        license = licenses.gpl2;
        homepage = "https://github.com/mate-desktop/libmateui";
        maintainers = [ maintainers.undefined ];
      };
    };
  in {
    packages.${system}.libmateui = libmateui;

    defaultPackage.${system} = libmateui;

    devShells.${system}.default = pkgs.mkShell {
      buildInputs = [
        pkgs.meson
        pkgs.ninja
        pkgs.pkg-config
        pkgs.gcc
        pkgs.glib
        pkgs.gtk3
        pkgs.xorg.libXScrnSaver
      ];
      shellHook = ''
        echo "dev shell for libmateui: run 'meson setup build && meson compile -C build'"
      '';
    };
  };
}