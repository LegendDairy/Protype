 #!/bin/bash
echo "Building IPL..."
nasm -I $HOME/Protype/ipl/include/ -f bin ipl/ipl.asm -o bin/ipl.sys
sudo losetup /dev/loop0 bin/floppy.img
echo "Injecting IPL..."
sudo mount -t msdos /dev/loop0 /media/floppy1/
sudo cp bin/ipl.sys /media/floppy1/
sudo umount /dev/loop0
sudo losetup -d /dev/loop0
echo "Done"
