#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load gcc/13.2.0
    module load openmpi/4.1.5
    module load cmake/3.22.2
fi

# red output
RED='\033[0;31m'
# no color
NC='\033[0m'

err () {
    echo -e "$RED$1$NC"
}

# help message to be output either with the -h flag or when using invalid syntax
showhelp () {
    printf "\033[31m%s\033[0m" "$0 - Build the HEP-CE Model"
    echo
    echo
    echo "Syntax: $(basename "$0") [-h|-t OPTION|-p|l]"
    echo "h              Print this help screen."
    echo "t OPTION       Set the build type to OPTION"
    echo "               Options: [Debug|Release|Build]"
    echo "               Default: Debug"
    echo "l              Build Shared Library."
    echo "p              Build and run tests."
}

dminstall () {
    if [[ ! -d "DataManagement" ]]; then
        git clone -b old_dm git@github.com:SyndemicsLab/DataManagement
    fi
    echo "DataManagement clone complete."

    # subshell needed to avoid changing working directory unnecessarily
    (
        cd "DataManagement" || return 1
        scripts/build.sh -i "$TOPLEVEL/lib/dminstall"
    )
    rm -rf DataManagement
}

# set default build type
BUILDTYPE="Debug"
BUILD_TESTS=""
BUILD_SHARED_LIBS="OFF"

# process optional command line flags
while getopts ":hplt:" option; do
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
        l)
            BUILD_SHARED_LIBS="ON"
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

    # ensure the `build/` directory exists
    ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"
    ([[ -d "bin/" ]] && rm -rf bin/*) || mkdir "bin/"
    ([[ -d "lib/" ]] && rm -rf lib/*.a && rm -rf lib/*.so && rm -rf lib/dminstall)

    # detect or install DataManagement
    if [[ ! -d "lib/dminstall" ]]; then
        if ! dminstall; then
            echo "Installing \`DataManagement\` failed."
            exit 1
        fi
    fi

    (
        cd "build" || exit

        CMAKE_COMMAND="cmake .. -DCMAKE_BUILD_TYPE=${BUILDTYPE}"

        # build tests, if specified
        if [[ -n "$BUILD_TESTS" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_TESTS=${BUILD_TESTS}"
            if [[ -f "$TOPLEVEL/bin/hepceTest" ]]; then
                rm "$TOPLEVEL/bin/hepceTest"
            fi
        fi

        # build static library if BUILD_STATIC_LIBRARY is on, otherwise build
        # shared library
        CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS"

        err "[EXECUTE] $CMAKE_COMMAND"

        $CMAKE_COMMAND
        (
            # determine the number of processing units available
            CORES="$(nproc --all)"
            # if CORES > 1 compile in parallel where possible
            ([[ (-n "$CORES") && ("$CORES" -gt "2") ]] && cmake --build . -j"$(( CORES - 2 ))") || cmake --build .
        )
    )
    # run tests, if they built properly
    if [[ (-n "$BUILD_TESTS") && (-f "bin/hepceTest") ]]; then
        bin/hepceTest
    fi
)
