### Mydm

A Simple display manager for Linux

```
cd path-to-mydm-src
./mkconfig.sh
make
sudo make install
```

For usage, run `mydm -h`

Runtime dep: glibc, libx11

Compiling dep: GNU GCC, GNU Make, libx11-dev (optional), shell, etc.

#### Do not want to install libx11-dev?

> Warning: it is not suggested! It is better if you install libx11-dev. And you can uninstall it after compiling.

When you compile mydm, libx11-dev may be needed because mydm use some headers of Xlib's.

But after compiling mydm, libx11-dev is not needed anymore.

And, if you don't want install to libx11-dev in compiling time, you can run this before make

```
./mkconfig.sh --own-libx11dev
```

But if you do it, you may get a error when make

```
/usr/bin/ld: cannot find -lX11
```

you can use `./lnlibx11.sh` to solve it, for example:

```
$ ./mkconfig.sh --own-libx11dev                       <= use the config
$ make                                                <= build mydm
  CC	mydm.o
  CC	tools.o
  CC	su.o
  LD	mydm
/usr/bin/ld: cannot find -lX11                        <= you get an error
collect2: error: ld returned 1 exit status
Makefile:17: recipe for target 'mydm' failed
make: *** [mydm] Error 1
$ ./lnlibx11.sh                                       <= just run it
Which libX11 shared object you want to link?\n
0: /usr/lib/arm-linux-gnueabihf/libX11.so.6
Your choose (default 0): 0                            <= choose a libX11.so
$ make                                                <= and then make again
  LD	mydm
  STRIP	mydm
$ ldd mydm                                            <= you can check it by ldd
	libX11.so.6 => /usr/lib/arm-linux-gnueabihf/libX11.so.6 (0xb6e4f000)
	libc.so.6 => /lib/arm-linux-gnueabihf/libc.so.6 (0xb6d61000)
	......
```
