#!/bin/bash

# Loop from 1 to 9
for i in {7..25}; do
    # Set points to the current loop value
    points=$i
    
    # Create a temporary file with Asymptote code to set points
    echo "int points = $points;" > params.asy
    
    # Run the Asymptote script, importing the parameter file and setting the output filename
    asy -o gen$((i+2)) params.asy main.asy
    
    # Remove the temporary parameter file
    rm params.asy
done

