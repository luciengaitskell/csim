{
  description = "csim - a fast vehicle sim";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            # https://github.com/NixOS/nixpkgs/issues/308482
            clang-tools # super important, otherwise clangd fails to find headers
            cmake
            ninja
            pkg-config
            eigen
          ];

          shellHook = ''
            export Eigen3_ROOT="${pkgs.eigen}"
          '';
        };
      }
    );
}
