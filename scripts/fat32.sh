image_name="AthenX-3.img"
image_size_mb=100  # Set the desired size in megabytes

# Create an empty disk image file
dd if=/dev/zero of="$image_name" bs=1M count="$image_size_mb"

# Format the disk image as FAT32
mkfs.fat -F 32 "$image_name"

echo "FAT32 disk image '$image_name' created successfully."