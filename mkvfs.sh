#!/bin/bash
echo 'mkvfs'
cd $1
gcc $1/src/tools/ramGenerator.c  -I src/ -o gen
./gen test.txt test.txt test2.txt  test2.txt
ld -m elf_i386 --build-id=none -r -b binary initrd.img -o init.o
cd -
