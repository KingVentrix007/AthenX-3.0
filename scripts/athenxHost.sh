image_file="AthenX.img"
sudo losetup /dev/loop1 "$image_file" -o 1048576
sudo mount /dev/loop1 /mnt/AthenX-3.0
sudo cp -r /mnt/AthenX-3.0/. drive_output/.
sudo umount /mnt/AthenX-3.0
sudo losetup -d /dev/loop1