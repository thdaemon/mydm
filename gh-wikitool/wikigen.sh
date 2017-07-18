#!/bin/bash

gen_md()
{
	#local filename=${2}/${1##*/}
	local tmp=`head -1 "$1"`
	tmp=${tmp#* }
	local filename=${2}/${tmp// /-}.md
	cat > $filename << EOF
**This file is generated from ${i} by script, you can directly access the original version in source code directory**

Generate time (UTC): `date -R -u`

EOF
	cat "$1" >> "$filename"
}

clonefrom=https://github.com/thdaemon/mydm.wiki.git
dirname=mydm.wiki

if ! [ -d "build_wiki" ]; then
	mkdir -p build_wiki
	(cd build_wiki; git clone --depth=1 "$clonefrom")
fi

outdir=build_wiki/${dirname}

for i in `find doc/ -type f`; do
	case ${i##*.} in
	md|markdown)
		gen_md "$i" "$outdir"
		;;
	*)
		echo "Unknow file type $i" >&2
		;;
	esac
done

(cd ${outdir}; git add .; git commit -m 'It is commited by wikigen.sh'; git push --all origin)

rm -rf build_wiki
