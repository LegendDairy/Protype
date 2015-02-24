echo "Building Pre-IPL"
cd $HOME/Protype/
nasm -I $HOME/Protype/boot/include/ -f bin boot/boot.asm -o if=bin/preipl.bin
sudo losetup /dev/loop0 bin/floppy.img
echo "Injecting Pre-IPL in floppy.img"
sudo dd if=bin/preipl.bin of=/dev/loop0 bs=512 count=2880
sudo losetup -d /dev/loop0
rm bin/preipl.bin
qemu-system-x86_64 -fda bin/floppy.img
