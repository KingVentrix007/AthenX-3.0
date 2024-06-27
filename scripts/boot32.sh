image_file="AthenX.img"

# Set the path to AthenX.bin
hackos_bin="AthenX.bin"
    dd if=/dev/zero of="$image_file" bs=1M count=1024
    echo -e "n\np\n1\n2048\n131071\nt\nc\na\n1\nw" | fdisk "$image_file"
    sudo losetup /dev/loop0 "$image_file"
    sudo losetup /dev/loop1 "$image_file" -o 1048576
    sudo mkdosfs -F32 -f 2 /dev/loop1
    sudo mount -o rw /dev/loop1 /mnt/AthenX-3.0

    # Copy your OS files and GRUB configuration
    # sudo cp -r "sysroot"/* /mnt/AthenX-3.0/
    sudo mkdir /mnt/AthenX-3.0/boot
    sudo cp AthenX.bin /mnt/AthenX-3.0/boot
    sudo cp AthenX.iso /mnt/AthenX-3.0/boot
    sudo grub-install --target=i386-pc --boot-directory=/mnt/AthenX-3.0 --root-directory=/mnt/AthenX-3.0 --force --no-floppy --modules="part_msdos fat" /dev/loop0
    echo "saved_entry=grub.cfg" | sudo tee /grub/grubenv
    sudo cp -r "grub/." "/mnt/AthenX-3.0/grub"
    sudo cp -r sysroot/. /mnt/AthenX-3.0/
    # Verify the root directory's LBA address using the file command

    # Unmount the image
    sudo umount /mnt/AthenX-3.0
    sudo losetup -d /dev/loop1
    sudo losetup -d /dev/loop0