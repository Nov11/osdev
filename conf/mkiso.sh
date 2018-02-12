#!/bin/bash
rm -rf iso
cd $1
mkdir -p iso/boot/grub
cp $2/myos.bin iso/boot
#cp $2/initrd.img iso/boot
cp conf/grub.cfg iso/boot/grub

grub-mkrescue -o myos.iso iso
echo 'made iso'