#!/bin/bash

# Check if arguments are provided
if [ $# -ne 2 ]; then
    echo "Usage: $0 <image_name> <image_size_mb>"
    exit 1
fi

image_name="$1"
image_size_mb="$2"

# Create an empty disk image file
dd if=/dev/zero of="$image_name" bs=1M count="$image_size_mb"

# Format the disk image as FAT32
# mkfs.fat -F 32 "$image_name"

echo "FAT32 disk image '$image_name' created successfully."
