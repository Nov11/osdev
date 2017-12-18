#!/bin/bash

if [ ! -d build ] ; then
mkdir build
cd build
cmake ..
make
cd ..
fi

qemu-system-i386 -show-cursor -cdrom myos.iso