nasm -I $HOME/Protype/ipl/include/ -f bin ipl/ipl.asm -o bin/ipl.sys
sudo losetup /dev/loop0 bin/floppy.img
sudo mount -t msdos /dev/loop0 /media/floppy1/
sudo cp bin/ipl.sys /media/floppy1/
sudo umount /dev/loop0
sudo losetup -d /dev/loop0
qemu-system-x86_64 -fda bin/floppy.img -cpu core2duo -smp cores=2,threads=1,sockets=1 -m 256