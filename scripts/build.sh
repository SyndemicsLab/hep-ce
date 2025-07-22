#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load gcc/13.2.0
    module load openmpi/4.1.5
    module load cmake/3.31.7
    module load ninja/1.10.2
    module load boost/1.83.0
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
    echo "               Options: [Debug|Release|Strict]"
    echo "               Default: Release"
}

sqliteinstall () {
    if [[ ! -d "SQLiteCpp" ]]; then
        git clone git@github.com:SRombauts/SQLiteCpp.git
    fi
    echo "SQLiteCpp clone complete."

    # subshell needed to avoid changing working directory unnecessarily
    (
        cd "SQLiteCpp" || return 1
        mkdir "build"
        cmake -B build .
        cmake --build build
        cmake --install build --prefix "$TOPLEVEL/lib/sqlitecpp"
    )
    rm -rf SQLiteCpp
}

# set default build type
BUILDTYPE="Release"

# process optional command line flags
while getopts ":ht:" option; do
    case $option in
        h)
            showhelp
            exit
            ;;
        t)
            case "$OPTARG" in
                "Debug"|"Release"|"Strict")
                    BUILDTYPE="$OPTARG"
                    ;;
                *)
                    echo "Specified build type is unrecognized!"
                    exit
                    ;;
            esac
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
    ([[ -d "lib/" ]] && rm -rf lib/*.a && rm -rf lib/*.so)

    # detect or install SQLiteCpp
    if [[ ! -d "lib/sqlitecpp" ]]; then
        if ! sqliteinstall; then
            echo "Installing \`SQLiteCpp\` failed."
            exit 1
        fi
    fi

    case $BUILDTYPE in
        "Debug")
            PRESET="gcc-debug-cluster"
            ;;
        "Release")
            PRESET="gcc-release-cluster"
            ;;
        "Strict")
            PRESET="gcc-release-strict-cluster"
            ;;
    esac
    cmake --workflow --preset "$PRESET" --fresh
)
