{
	stdenv,
	cmake,
	libadt,
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
