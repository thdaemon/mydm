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

#### libx11-dev

When you compile mydm, libx11-dev may be needed because mydm use some headers of Xlib's.

But after compiling mydm, libx11-dev is not needed anymore.

And, if you don't want install libx11-dev in compiling time, you can run this before make

```
./mkconfig.sh --own-libx11dev
```
