Do something to keep me occupied.o

The original reference project is here:[link](http://www.jamesmolloy.co.uk/tutorial_html/1.-Environment%20setup.html).
I did a large portion of that but stopped at dynamic memory management as I can recall.
I have lots of time to kill, why not finish it?

Some hints while redo this project:
* I was coding on a ubuntu 32bit OS back then. So if anyone works on a 64bit OS for now just like I do,
 maybe you can save the effort of building a cross-compiler target 32bit machine by running this:```sudo apt install gcc-multilib```.


* --build-id=none is needed or grub cannot find the multiboot header.

* How to use gdb remote debug to connect qemu instance in Clion? I haven't combine the building process with debugging yet.
Config external target before initiating remote debugging. Use -daemonize for starting qemu or Clion will not proceed.Set program as 'qemu-system-i386' and
parameters as '-cdrom path_to_myos.iso -s -S -daemonize'.


Something not known yet. Better jotting them here as I may forget them in the future. :
* How to find the absolute address of a symbol in the final elf file? E.g. Locate the multiboot header magic value.
This will help when grub says the image is not bootable, which involves the build-id linker option.


