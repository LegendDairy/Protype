 #!/bin/bash
./boot/build.sh
./ipl/build.sh

cd $HOME/Protype/kernel
make
rm *.o
cd $HOME/Protype
sudo losetup /dev/loop0 bin/floppy.img
sudo mount -t msdos /dev/loop0 /media/floppy1/
sudo cp bin/kernel.sys /media/floppy1/
sudo umount /dev/loop0
sudo losetup -d /dev/loop0
sudo rm bin/kernel.sys
