#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    # module load gcc/12.2.0
    # module load openmpi/4.1.5
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

dminstall () {
    if [[ ! -d "DataManagement" ]]; then
	git clone git@github.com:SyndemicsLab/DataManagement
    fi
    # subshell needed to avoid changing working directory unnecessarily
    (
	cd "DataManagement" || return 1
	scripts/build.sh -l "$TOPLEVEL/lib/dminstall"
	if [[ ! -e "lib/libDataManagement.so" ]]; then
	    (
		cd "$TOPLEVEL" || return
		rm -rf lib/* DataManagement
	    )
	    return 1
	fi
	# shared object installation is still not like it was before, so these
	# lines accomplish the same purpose. Will require more CMakeFile
	# troubleshooting in DataManagement in the future.
	mkdir -p "$TOPLEVEL/lib/dminstall"
	cp -r "include" "lib" "$TOPLEVEL/lib/dminstall"
	mkdir -p "$TOPLEVEL/lib/dminstall/lib/cmake/DataManagement"
	cp "build/"*".cmake" "$TOPLEVEL/lib/dminstall/lib/cmake/DataManagement"
	cp "build/CMakeFiles/Export/"*"/DataManagementTargets"*".cmake" "$TOPLEVEL/lib/dminstall/lib/cmake/DataManagement"
    )
    rm -rf DataManagement
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

# load conda environment
echo "Checking if \`conda\` is found..."
# ensure conda is present on the system
if ! command -v conda &>/dev/null; then
    echo "\`conda\` not present on the system! Exiting..."
    exit 1
else
    echo "\`conda\` found!"
fi

(
    # change to the top-level git folder
    TOPLEVEL="$(git rev-parse --show-toplevel)"
    cd "$TOPLEVEL" || exit

    # ensure the `build/` directory exists
    ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"
    ([[ -d "bin/" ]] && rm -rf bin/*) || mkdir "bin/"
    ([[ -d "lib/" ]] && rm -rf lib/*.a)

    # detect or install DataManagement
    if [[ ! -d "lib/dminstall" ]]; then
	if ! dminstall; then
	    echo "Installing \`DataManagement\` failed."
	    exit 1
	fi
    fi

    # load conda environment
    if [[ -f "$(conda info --base)/etc/profile.d/conda.sh" ]]; then
	# shellcheck source=/dev/null
	source "$(conda info --base)/etc/profile.d/conda.sh"
    fi
    if ! conda info --envs | grep '^hepce' >/dev/null; then
	conda env create -f "environment.yml" -p "$(conda config --show envs_dirs | awk '/-/{printf $NF;exit;}')/hepce"
    fi
    conda activate hepce

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
