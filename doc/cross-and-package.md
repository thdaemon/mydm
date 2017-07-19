## Cross compiling and building packages

> It only applies to linux, does not apply to freebsd

### Cross compiling

You can use `CROSS` to set the cross compiler, for example

```
make CROSS=arm-linux-gnueabihf-
```

You must have dependent libraries for target architecture, e.g. libx11 and libx11-dev

### Cross compiling for 32bit on 64bit

> It is suitable for compiling the i386 version on the amd64 platform

building a 32bit version may need another method, you need gcc-multilib and set `CC`, for example

```
make CC="gcc -m32"
```

You must have dependent libraries for target architecture, e.g. libx11 and libx11-dev

### Build binary archives

Run

```
make bin-tar
```

And it will ask you the target machine architecture name, enter it (such as i386) and press Enter.

If you have compiled the greeter, then the binary package will be with greeter

### Build Debain packages

Run

```
make deb
```

And it will ask you the target machine architecture name, enter it (such as i386) and press Enter.

If you have compiled the greeter, then the binary package will be with greeter

** WARNING You may need change deb depends**

If the version of gcc is newer, mydm may need newer glibc. So under this circumstance, you need change `pkgsrc/deb_depends` to change depends.

If the version of libgtk is newer, you need change `greeter/.../pkgsrc/deb_depends` to change depends.
