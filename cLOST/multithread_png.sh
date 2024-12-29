#!/bin/bash

# Check if a directory is provided as an argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

# Get the directory from the command line argument
DIR="$1"

# Check if the directory exists
if [ ! -d "$DIR" ]; then
    echo "Error: Directory '$DIR' does not exist."
    exit 1
fi

# Loop through all files in the directory and run the Python script in the background
for file in "$DIR"/*; do
    if [ -f "$file" ]; then
        python png_to_rgb.py "$file" &
    fi
done

# Wait for all background processes to finish
wait

echo "All processes completed."
