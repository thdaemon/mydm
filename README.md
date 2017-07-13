## Mydm

A simple and lightweight display manager for Linux/Unix

```
cd path-to-mydm-src
./mkconfig.sh --enable-xsec
make
sudo make install
```

On FreeBSD, you need use `gmake` instead of `make`.

And if you do not want to install gcc, you can use FreeBSD clang compiler

```
gmake CC="cc -I/usr/local/include" LDFLAGS="-lX11 -L/usr/local/lib"
```

For usage, run `mydm -h`

**Runtime dependent**: glibc, libx11

**Compiling dependent**: GNU Make, GCC or other c99-supported compilers, libx11-dev (optional), shell, etc.

### Binary Release

mydm has .tar.xz format binary archive and .deb format Debian Package.

[Go to the release page](https://github.com/thdaemon/mydm/releases)

### Cross compiling and building packages

If you want to learn to cross compile this project or build a binary package, please see [this Wiki](doc/cross-and-package.md)

### XSecurity

1. Not Authority

	mydm can run X without XSecurity, and this is the default. It is NOT suitable for computers that have multiple users logged in at the same time. But for the personal computer and most of the embedded device is enough.

2. Use MIT-MAGIC-COOKIE-1

	> Tip: If you run `./mkconfig.sh` without `--enable-xsec` at compile time, mydm will not support it.

	You can use `-A` option to enforce the authority method. For more about MIT-MAGIC-COOKIE-1, see Xsecurity(7) manpage.

### Set the default X Client

X Client can be set at runtime by cmdline, but you can set a default X client before build

If you do not set it, default X client will be set to 'xterm'

```
./mkconfig.sh --enable-xsec --default-xclient=startxfce4
```

### Do not want to install libx11-dev (Linux only)?

> Warning: it is not suggested! It is better if you install libx11-dev. And you can uninstall it after compiling.

When you compile mydm, libx11-dev may be needed because mydm use some headers of Xlib's. But after compiling mydm, libx11-dev is not needed anymore.

So, if you don't want install to libx11-dev in compiling time, you can see [this Wiki](doc/own-libx11dev.md)
