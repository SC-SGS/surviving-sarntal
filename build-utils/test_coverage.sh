#!/bin/bash

# Remove the build directory if it exists
rm -rf cmake-build-debug

#!/bin/bash

# Determine platform
case "$OSTYPE" in
    darwin*)
        # macOS: Use Unix Makefiles and enable coverage
        cmake -S . -B cmake-build-debug -G "Unix Makefiles" -DENABLE_COVERAGE=ON
        ;;
    linux-gnu*)
        # Linux: Drop the Unix Makefiles generator (let CMake choose the default)
        cmake -S . -B cmake-build-debug -DENABLE_COVERAGE=ON
        ;;
    *)
        echo "Unsupported platform: $OSTYPE"
        exit 1
        ;;
esac

# Build the project
cmake --build cmake-build-debug

# Run the tests
if [ -d "./cmake-build-debug/test" ]; then
  cd ./cmake-build-debug/test || exit
  if [ -f "./test-surviving-sarntal" ]; then
    ./test-surviving-sarntal
  else
    echo "Error: test-surviving-sarntal binary not found!"
    exit 1
  fi
  cd - || exit  # Return to the previous directory
else
  echo "Error: Test directory not found!"
  exit 1
fi

# Generate the coverage report
cmake --build cmake-build-debug --target coverage
