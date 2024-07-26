#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load gcc/12.2.0
    module load openmpi/4.1.5
    module load miniconda
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
    TOPLEVEL="$(git rev-parse --show-toplevel)"
    cd "$TOPLEVEL" || exit

    # load conda environment
    if [[ -f "$(conda info --base)/etc/profile.d/conda.sh" ]]; then
	# shellcheck source=/dev/null
	source "$(conda info --base)/etc/profile.d/conda.sh"
    fi
    if ! conda info --envs | grep '^hepce' >/dev/null; then
	conda create -f "environment.yml"
    fi
    conda activate hepce

    # ensure the `build/` directory exists
    ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"
    ([[ -d "bin/" ]] && rm -rf bin/*) || mkdir "bin/"
    ([[ -d "lib/" ]] && rm -rf lib/*.a)

        # detect or install DataManagement
    if [[ ! -d "lib/dminstall" ]]; then
	git clone git@github.com:SyndemicsLab/DataManagement
	if ! (
		cd "DataManagement" || exit 1
		./install.sh -i "$TOPLEVEL/lib/dminstall"
	    ); then
	    echo "Installing \`DataManagement\` failed."
	fi
	rm -rf DataManagement
    fi

    (
	cd "build" || exit

	CMAKE_COMMAND="cmake .. -DCMAKE_BUILD_TYPE=$BUILDTYPE"

	# build tests, if specified
	if [[ -n "$BUILD_TESTS" ]]; then
	    CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_TESTS=$BUILD_TESTS"
	fi

	$CMAKE_COMMAND
	(
	    # determine the number of processing units available
	    CORES="$(nproc --all)"
	    # if CORES > 1 compile in parallel where possible
	    ([[ -n "$CORES" ]] && cmake --build . -j"$CORES") || cmake --build .
	)
    )
    # run tests, if they built properly
	if [[ (-n "$BUILD_TESTS") && (-f "bin/hepceTest") ]]; then
	    bin/hepceTest
	fi
)
