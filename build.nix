{
	fetchFromGitHub,
	stdenv,
	cmake,
	libadt ? import (fetchFromGitHub {
		owner = "TheMadman";
		repo = "libadt";
		rev = "3ab87a6a9f02f9a1fed35e6c5c4449729b8cc795";
		hash = "sha256-5nL1ktDEG2LNTl+0fgoIt8l+PSSDssM30Cnu382uJjs=";
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
