## Mydm

A simple and lightweight display manager for Linux

```
cd path-to-mydm-src
./mkconfig.sh
make
sudo make install
```

For usage, run `mydm -h`

**Runtime dependent**: glibc, libx11

**Compiling dependent**: GNU GCC, GNU Make, libx11-dev (optional), shell, etc.

### Xauth

mydm not support Xauth now, so mydm is NOT suitable for computers that have multiple users logged in at the same time.

But for the personal computer and most of the embedded device is enough.

### Set the default X Client

X Client can be set at runtime by cmdline, but you can set a default X client before build

If you do not set it, default X client will be set to 'startxfce4'

```
./mkconfig.sh --default-xclient=gnome-session
```

### Do not want to install libx11-dev?

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
                                                         (on 64bit system you may
                                                          get two resaults)
$ make                                                <= and then make again
  LD	mydm
  STRIP	mydm
$ ldd mydm                                            <= you can check it by ldd
	libX11.so.6 => /usr/lib/arm-linux-gnueabihf/libX11.so.6 (0xb6e4f000)
	libc.so.6 => /lib/arm-linux-gnueabihf/libc.so.6 (0xb6d61000)
	......
```

Tip: lnlibx11.sh does not create or modify files outside the source directory.
