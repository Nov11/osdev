Do something to keep me occupied.o

The original project is here:[link](http://www.jamesmolloy.co.uk/tutorial_html/1.-Environment%20setup.html).
I did a large portion of that but stopped at dynamic memory management as I can recall.
As I have lots of time to kill, why not finish it?

Some hints while redo this project:
* I was coding on a ubuntu 32bit OS back then. So if anyone works on a 64bit OS for now just like I do,
 maybe you can save the effort of building a cross-compiler target 32bit machine by running this:```sudo apt install gcc-multilib```.


* --build-id=none is needed or grub cannot find the multiboot header.

Something not known yet:
* how to find the absolute address of a symbol in the final elf file? e.g. locate the multiboot header magic value.


