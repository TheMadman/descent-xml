{
	fetchFromGitHub,
	stdenv,
	cmake,
	libadt ? import (fetchFromGitHub {
		owner = "TheMadman";
		repo = "libadt";
		rev = "6ab226d4f0b5fb91d00d81810b60e82a88f1fd99";
		hash = "sha256-87YhgCmCBvJaZ2w+I1BKILUOSuF8/Isip3FWAPDYZIQ=";
	}),
}:

stdenv.mkDerivation {
	pname = "descent-xml";
	version = "0.0.1";
	src = ./.;

	nativeBuildInputs = [cmake];
	buildInputs = [
		libadt
	];

	cmakeFlags = [
		"-DBUILD_TESTING=True"
	];

	doCheck = true;
	checkTarget = "test";
}
