#!/usr/bin/env bash

read -p "The arch? (e.g. i386, armhf, amd64, etc): " arch

[ "$arch" = '' ] && exit 1

name="mydm-linux-glibc-${arch}-`cat version`"

rm -rf build-package/${name} build-package/${name}.tar.xz
mkdir -p build-package/${name}

make PREFIX="`pwd`/build-package/${name}/" install

if [ -x "greeters/mydm-gtk-demo-greeter/mydm-gtk-demo-greeter" ]; then
	make PREFIX="`pwd`/build-package/${name}/" gtk_greeter_install
fi

cat > build-package/${name}/README << EOF
Git repository: https://github.com/thdaemon/mydm

Version: `cat version`

WARNING: This binary version may work incorrectly on your system, so if you encounter problems, you should build this project from source at first.
EOF

(cd build-package; tar cvJf "${name}.tar.xz" "${name}/")
rm -r build-package/${name}
