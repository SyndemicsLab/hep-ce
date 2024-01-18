#!/usr/bin/bash

# check-submodules () {
#     # ensure that the submodules are included in the config
#     git submodule init
#     # check if the submodules update without error, otherwise display a message.
#     if ! git submodule update --recursive; then
# 	echo "There was an issue trying to load git submodules."
#     fi
# }

# #ensure the "build/" directory exists
# ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"

# # check if conan exists in the current environment
# if ! command -v conan &>/dev/null; then
#     echo "The \`conan\` command is not found!"
#     exit 1
# fi

# # ensure that DataManagement is present before trying to build
# check-submodules

# use conan to build dependencies if not found on the current system
conan install . --build=missing --settings=build_type=Debug

# use a subshell to enter the build folder and build HEP-CE
(
    cd "build/" || exit
    # check if the conan generator file was generated successfully
    if [[ -f "Debug/generators/conanbuild.sh" ]]; then
	source "Debug/generators/conanbuild.sh"
    else
	echo "\`conan\` generator failed. Terminating."
	exit 1
    fi
    cmake .. -DCMAKE_PREFIX_PATH=/home/matt/Repos/DataManagement/_install/lib/cmake/DataManagement -DCMAKE_TOOLCHAIN_FILE=Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DBUILD_SHARED_LIBS=YES
    # use another subshell to build the model, using as many processors as
    # possible
    (
	# determine the number of processing units available
	CORES="$(nproc --all)"
	# if CORES > 1 compile in parallel where possible
	([[ -n "$CORES" ]] && cmake --build . -j"$CORES") || cmake --build .
    )
    # deactivate the conan virtual environment
    source "Debug/generators/deactivate_conanbuild.sh"
    # run tests, if they built properly
    if [[ -f tests/hepceTest ]]; then
	tests/hepceTest
    fi
)
