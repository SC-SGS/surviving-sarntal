#!/bin/bash

# Path to your executable file
executable="cmake-build-debug/surviving-sarntal"

# timeout with seconds, executing the file
timeout 5s $executable &

# Get the PID (Process ID) of the executable
pid=$!
status_code=$?

# Wait for the executable to finish or timeout
if wait $pid
then
  echo "Executable ran with exit code: ${status_code}"
  #echo "Executable encountered an error or exceeded the timeout."
  #$status_code == 124
else
  echo "Executable ran with exit code: ${status_code}"
  #echo "Executable ran successfully without errors."
fi