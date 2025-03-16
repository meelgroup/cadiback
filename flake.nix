{
  description = "BackBone Extractor for CaDiCal";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    cadical = {
      url = "github:itepastra/cadical/add-flake";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };
  outputs =
    {
      self,
      nixpkgs,
      cadical,
    }:
    let
      inherit (nixpkgs) lib;
      systems = lib.intersectLists lib.systems.flakeExposed lib.platforms.linux;
      forAllSystems = lib.genAttrs systems;
      nixpkgsFor = forAllSystems (system: nixpkgs.legacyPackages.${system});
      fs = lib.fileset;

      cadiback-package =
        {
          stdenv,
          fetchFromGitHub,
          git,
          cadical,
        }:
        stdenv.mkDerivation {
          name = "cadiback";
          src = fs.toSource {
            root = ./.;
            fileset = fs.unions [
              ./configure
              ./makefile.in
              ./test
              ./cadiback.h
              ./cadiback.cpp
              ./generate
              ./VERSION
            ];
          };

          nativeBuildInputs = [ git ];
          buildInputs = [ cadical ];

          patchPhase = ''
            substituteInPlace makefile.in \
              --replace-fail "/usr/" "$out/" \
              --replace-fail "../cadical" "${cadical}"
            substituteInPlace generate \
              --replace-fail ${lib.escapeShellArg ''[ -d .git ] || die "could not find '.git' directory"''} "" \
              --replace-fail ${lib.escapeShellArg "`git show|head -1|awk '{print $2}'`"} ${nixpkgs.rev}
            cat generate
          '';
          configurePhase = ''
            export CADICAL=${cadical}
            ./configure
          '';

          preInstall = ''
            mkdir -p $out/lib
            mkdir -p $out/include
          '';
        };

    in
    {
      packages = forAllSystems (
        system:
        let
          cadiback = nixpkgsFor.${system}.callPackage cadiback-package {
            cadical = cadical.packages.${system}.cadical;
          };
        in
        {
          inherit cadiback;
          default = cadiback;
        }
      );
    };
}
