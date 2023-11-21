#!/usr/bin/bash

# ensure the "build/" directory exists
([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"

# use a subshell to enter the build folder and build HEP-CE
(
    conan install . --build=missing --settings=build_type=Debug

    cd "build/"

    source Debug/generators/conanbuild.sh

    cmake .. -DCMAKE_TOOLCHAIN_FILE=Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
    # use another subshell to build the model
    (
	# determine the number of processing units available
	CORES="$(nproc --all)"
	# if CORES > 1 compile in parallel where possible
	([[ -n "$CORES" ]] && cmake --build . -j"$CORES") || cmake --build .
    )

    source Debug/generators/deactivate_conanbuild.sh
    tests/hepceTest
)
