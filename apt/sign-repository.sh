#!/bin/sh

script_base_dir=`dirname $0`

if [ $# != 1 ]; then
    echo "Usage: $0 DISTRIBUTIONS"
    echo " e.g.: $0 'debian ubuntu'"
    exit 1
fi

DISTRIBUTIONS=$1

run()
{
    "$@"
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

for distribution in ${DISTRIBUTIONS}; do
    for target in $(cat ${distribution}/TARGETS); do
	release=${distribution}/dists/${target}/Release
	rm -f ${release}.gpg
	gpg --sign -ba -o ${release}.gpg ${release}
    done;
done
