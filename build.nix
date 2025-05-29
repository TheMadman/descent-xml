{
	fetchFromGitHub,
	stdenv,
	cmake,
	libadt ? import (fetchFromGitHub {
		owner = "TheMadman";
		repo = "libadt";
		rev = "4120f368a28ca61c7e765476ae5d252ad3dea9c9";
		hash = "sha256-8sO+IjopVu80oJmk1yykm9Bynh2QIDH0yl/ObDgQrSc=";
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
