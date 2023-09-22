#!/bin/bash

# path to executable file
executable = "cmake-build-debug/surviving-sarntal"

# maximum execution time (in seconds)
time = 30

$executable &

pid = $!

if wait $pid; then
  echo "Executable ran successfully without errors."
else
  echo "Executable encountered an error or exceeded the timeout"
fi