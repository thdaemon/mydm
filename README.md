## Mydm

A simple and lightweight display manager for Linux/Unix

**Runtime dependent**: Glibc, libX11

**Compiling dependent**: GNU Make, GCC or other c99-supported compilers, libX11-dev (optional), shell, etc.

```
cd path-to-mydm-src
./mkconfig.sh --enable-xsec
make
sudo make install
```

On FreeBSD, you need use `gmake` instead of `make`. And if you do not want to install gcc, you can use FreeBSD clang compiler

```
gmake CC="cc -I/usr/local/include" LDFLAGS="-lX11 -L/usr/local/lib"
```

To build a debug version, you can use

```
DEBUG=1 make
```

### Binary Release

mydm has .tar.xz format binary archive and .deb format Debian Package.

[Go to the release page](https://github.com/thdaemon/mydm/releases)

### Usage

For a simple example, start the Xfce4 desktop on display: 0, the 7th virtual console as a user foo

```
# mydm -d :0 -v vt7 -c startxfce4 -u foo -n
```

Greeter mode and XSecurity please see below.

Some commonly used options

```
-d display         Display name, default ':0'
-v vt              VT number, default 'vt7'
-c program         X client, e.g. 'xterm'
-r program         After run x client, will run it, but do not wait it to exit, default null
-u(-l) username    The user to login, default null (Not login)
-n                 Do not use the su command of system (default used)
-A                 Use MIT-MAGIC-COOKIE-1 XSecurity, See below!
-g                 Use greeter mode (After session exited restart it), See below!
```

Add server options

You can add the additional options to X server. e.g. -depth x, like this

```
# mydm -d :0 -v vt7 -c startxfce4 -u foo -n -- -depth 24
```

For more usages, please run `mydm -h`

### XSecurity

1. Not Authority

	mydm can run X without XSecurity, and this is the default. It is NOT suitable for computers that have multiple users logged in at the same time. But for the personal computer and most of the embedded device is enough.

2. Use MIT-MAGIC-COOKIE-1

	> Tip: If you run `./mkconfig.sh` without `--enable-xsec` at compile time, mydm will not support it.

	You can use `-A` option to enforce the authority method. For more about MIT-MAGIC-COOKIE-1, see Xsecurity(7) manpage.

### Greeter mode

Mydm supports greeter mode, you can use the `-g` option to activate the mode

```
# mydm -c /usr/lib/mydm/mydm-gtk-demo-greeter/mydm-gtk-demo-greeter -g
```

On greeter mode, when a session exited, mydm will restart it.

So, greeter is usually a gui program, it created the login interface requires the user to enter the user name and password. I wrote a greeter demo, use gtk+, see [here](greeters/mydm-gtk-demo-greeter)

To compile and install it, you can run these in top source directory

```
make gtk_greeter
sudo make gtk_greeter_install
```

### Cross compiling and building packages

If you want to learn to cross compile this project or build a binary package, please see [this Wiki](doc/cross-and-package.md)

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
