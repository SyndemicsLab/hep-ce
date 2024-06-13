#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load python3/3.10.12
    module load gcc/12.2.0
fi

# help message to be output either with the -h flag or when using invalid syntax
showhelp () {
    printf "\033[31m%s\033[0m" "$0 - Build the HEP-CE Model"
    echo
    echo
    echo "Syntax: $(basename "$0") [-h|-t OPTION|-p]"
    echo "h              Print this help screen."
    echo "t OPTION       Set the build type to OPTION"
    echo "               Options: [Debug|Release|Build]"
    echo "               Default: Debug"
    echo "p              Build and run tests."
}

# set default build type
BUILDTYPE="Debug"
BUILD_TESTS=""

# process optional command line flags
while getopts ":hpt:" option; do
    case $option in
	h)
	    showhelp
	    exit
	    ;;
	t)
	    case "$OPTARG" in
		"Debug"|"Release")
		    BUILDTYPE="$OPTARG"
		    ;;
		*)
		    echo "Specified build type is invalid!"
		    exit
		    ;;
	    esac
	    ;;
	p)
	    BUILD_TESTS="ON"
	    ;;
	\?)
	    echo "Error: Invalid option flag provided!"
	    showhelp
	    exit
	    ;;
    esac
done

(
    # change to the top-level git folder
    cd "$(git rev-parse --show-toplevel)" || exit
    CONANPATH=$(command -v conan)

    # install conan, if not found in the current scope
    if [[ -z "$CONANPATH" ]]; then
	echo "The \`conan\` command is not found\! Attempting a local installation..."
	if [[ -z "$VIRTUAL_ENV" ]]; then
	    if ! pip install --user conan; then
		echo "\`conan\` installation failed. Exiting."
		exit 1
	    fi
	else
	    if ! pip install conan; then
		echo "\`conan\` installation failed. Exiting."
		exit 1
	    fi
	fi
	CONANPATH="python3 -m conans.conan"
    fi

    # ensure the `build/` directory exists
    ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"

    # install dependencies via conan
    if [[ ! -f "$HOME/.conan2/profiles/default" ]]; then
	$CONANPATH profile detect --force
    fi

    $CONANPATH install . --build=missing --settings=build_type="$BUILDTYPE"

    (
	cd "build" || exit
	# check if the conan generator file was generated successfully
	if [[ -f "$BUILDTYPE/generators/conanbuild.sh" ]]; then
	    # shellcheck source=/dev/null
	    source "$BUILDTYPE/generators/conanbuild.sh"
	else
	    echo "\`conan\` generator failed. Terminating."
	    exit 1
	fi

	# build tests, if specified
	if [[ -n "$BUILD_TESTS" ]]; then
	    cmake .. -DCMAKE_TOOLCHAIN_FILE="$BUILDTYPE"/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE="$BUILDTYPE" -DBUILD_TESTS="$BUILD_TESTS"
	else
	    cmake .. -DCMAKE_TOOLCHAIN_FILE="$BUILDTYPE"/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE="$BUILDTYPE"
	fi
	(
	    # determine the number of processing units available
	    CORES="$(nproc --all)"
	    # if CORES > 1 compile in parallel where possible
	    ([[ -n "$CORES" ]] && cmake --build . -j"$CORES") || cmake --build .
	)
	# deactivate the conan virtual environment
	# shellcheck source=/dev/null
	source "$BUILDTYPE/generators/deactivate_conanbuild.sh"
	# run tests, if they built properly
    cd ..
	if [[ (-n "$BUILD_TESTS") && (-f "bin/hepceTest") ]]; then
	    bin/hepceTest
	fi
    )
)
