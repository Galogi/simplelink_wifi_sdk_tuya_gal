#!/bin/bash

# 1. Define the root destination folder
DEST_ROOT="./include"
mkdir -p "$DEST_ROOT"

echo "Reorganizing headers into $DEST_ROOT/{feature}/..."

# 2. Find all 'include' directories
# ! -path "./include/*" ensures we don't try to process the destination folder itself
find . -type d -name "include" ! -path "$DEST_ROOT*" | while read -r inc_dir; do

    # Check if .h files exist in this folder
    if ls "$inc_dir"/*.h 1> /dev/null 2>&1; then
        
        # 3. Extract the "feature" name (the name of the parent folder)
        # Example: ./src/network/mqtt/include  -> feature_name = "mqtt"
        parent_path="$(dirname "$inc_dir")"
        feature_name="$(basename "$parent_path")"
        
        # 4. Create the specific destination subfolder
        target_dir="$DEST_ROOT/$feature_name"
        mkdir -p "$target_dir"
        
        echo "Processing feature: [$feature_name]"
        echo "  From: $inc_dir"
        echo "  To:   $target_dir"

        # 5. Move the header files
        mv -n "$inc_dir"/*.h "$target_dir/"
        
        # 6. Remove the old include folder if it is now empty
        rmdir "$inc_dir" 2> /dev/null
    fi

done

echo "Done. Your headers are now organized by feature."