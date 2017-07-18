#!/bin/bash

clonefrom=https://github.com/thdaemon/mydm.wiki.git
dirname=mydm.wiki

if ! [ -d "build_wiki" ]; then
	mkdir -p build_wiki
	(cd build_wiki; git clone --depth=1 "$clonefrom")
fi

outdir=build_wiki/${dirname}

for i in `find doc/ -type f`; do
	filename=${outdir}/${i##*/}
	cat > $filename << EOF
**This file is generated from ${i} by script, you can directly access the original version in source code dir**

Generate time (UTC): `date -R -u`

EOF
	cat $i >> $filename
done

(cd ${outdir}; git add .; git commit -m 'It is commited by wikigen.sh'; git push --all origin)

rm -rf build_wiki
