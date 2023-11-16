#!/usr/bin/bash

# # create the build directory if it doesn't exist, otherwise ensure it's empty
# ([[ -d build ]] && rm -rf build/*) || mkdir build
# # create a conan profile based on current environment
# conan profile detect --force
# # install missing dependencies
# conan install . --output-folder=build --build=missing -s build_type=Debug -s compiler.cppstd=gnu14
# # enter a subshell and build the model
# (
#    cd build || exit
#    cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
#    cmake --build . --config Debug
#    # run code tests if they built
#    [[ -f test/respondTest ]] && test/respondTest
# )
cd ../
conan install . --build=missing --settings=build_type=Debug
cd build
source Debug/generators/conanbuild.sh
cmake .. -DCMAKE_TOOLCHAIN_FILE=Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build .
source Debug/generators/deactivate_conanbuild.sh
./tests/hepceTest
