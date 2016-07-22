 #!/bin/bash
./boot/build.sh
./ipl/build.sh

nasm -f bin kernel/cpu/ap_init.asm -o bin/apbm.prx

cd $HOME/Protype/kernel
make
make clean
cd $HOME/Protype
sudo losetup /dev/loop0 bin/floppy.img
sudo mount -t msdos /dev/loop0 /media/floppy1/
sudo cp bin/kernel.sys /media/floppy1/
sudo umount /dev/loop0
sudo losetup -d /dev/loop0
echo "Done"
