#!/bin/bash

# Cross-platform test script

# Ensure the project is built
source build-utils/build.sh

# Refer to the coverage scripts if you want to add coverage testing

# mkdir -p cmake-build-debug/coverage
# lcov -c -i -d cmake-build-debug/src -o cmake-build-debug/coverage/base.info

# Run tests
cd ./cmake-build-debug/test || exit

if [[ "$OSTYPE" == "cygwin"* || "$OSTYPE" == "msys"* || "$OSTYPE" == "win32"* ]]; then
    ./test-surviving-sarntal.exe
else
    ./test-surviving-sarntal
fi

cd ../..
