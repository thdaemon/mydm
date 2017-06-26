#!/usr/bin/env bash

read -p "The arch? (e.g. i386, armhf, amd64, etc): " arch

[ "$arch" = '' ] && exit 1

name="mydm-linux-glibc-${arch}"

rm -rf build-package/${name}*
mkdir -p build-package/${name}/usr/local/bin/
mkdir -p build-package/${name}/DEBIAN

cp mydm build-package/${name}/usr/local/bin/mydm

cat > build-package/${name}/DEBIAN/control << EOF
Package: mydm
Version: `cat version`
Architecture: ${arch}
Maintainer: Tian Hao <thxdaemon@gmail.com>
Depends: `cat pkgsrc/deb_depends`
Section: base
Priority: extra
Homepage: https://github.com/thdaemon/mydm
Description: Mydm - A simple and lightweight display manager for Linux
 For usage, run mydm -h
 ========================
 WARNING: This binary version may work incorrectly on your system, so if you encounter problems, you should build this project from source at first.
 ========================
 About XSecurity
 1. Not Authority: mydm can run X without XSecurity, and this is the default. It is NOT suitable for computers that have multiple users logged in at the same time. But for the personal computer and most of the embedded device is enough.
 2. Use MIT-MAGIC-COOKIE-1: You can use -A option to enforce the authority method. For more about MIT-MAGIC-COOKIE-1, see Xsecurity(7) manpage.
EOF

(cd build-package; dpkg-deb -b "${name}" "${name}.deb")
rm -r build-package/${name}
