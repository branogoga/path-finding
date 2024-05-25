#!/bin/bash

# Recursively find all .dot files and print their paths along with corresponding .png paths
find . -type f -name "*.dot" | while read -r dot_file; do
    png_file="${dot_file%.dot}.png"
    echo "$dot_file" 
    echo "$png_file"
    fdp -Tpng "$dot_file" -o "$png_file"
done