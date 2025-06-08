let
	libadt = import (pkgs.fetchFromGitHub {
		owner = "TheMadman";
		repo = "libadt";
		rev = "6ab226d4f0b5fb91d00d81810b60e82a88f1fd99";
		hash = "sha256-87YhgCmCBvJaZ2w+I1BKILUOSuF8/Isip3FWAPDYZIQ=";
	});
	pkgs = import <nixpkgs> {};
in
pkgs.callPackage ./build.nix { inherit libadt; }
