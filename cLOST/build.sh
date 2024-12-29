#!/bin/bash

# Set the output executable name
OUTPUT_EXEC="program"

# List of source files (add/remove filenames as needed)
SRC_FILES=(
    "attitude.c"
    "star_identifier.c"
    "utils.c"
    "main.c"
    "catalog.c"
    "centroider.c"
    "linalg.c"
    "timer.c"
)

# List of object files (derived from the source files)
OBJ_FILES=()
for src in "${SRC_FILES[@]}"; do
    OBJ_FILES+=("${src%.c}.o")
done

# Compile each .c file into a .o file
echo "Compiling source files..."
for src in "${SRC_FILES[@]}"; do
    gcc -c "$src" -o "${src%.c}.o" -O3
    if [ $? -ne 0 ]; then
        echo "Compilation failed for $src"
        exit 1
    fi
done

# Link the .o files into the final executable
echo "Linking object files..."
gcc "${OBJ_FILES[@]}" -o "$OUTPUT_EXEC" -O3
if [ $? -ne 0 ]; then
    echo "Linking failed"
    exit 1
fi

echo "Build successful! Output executable: $OUTPUT_EXEC"
