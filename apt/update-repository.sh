#!/bin/sh

script_base_dir=`dirname $0`

if [ $# != 1 ]; then
    echo "Usage: $0 DISTRIBUTIONS ARCHITECTURES"
    echo " e.g.: $0 'debian ubuntu' 'i386 amd64'"
    exit 1
fi

DISTRIBUTIONS=$1
ARCHITECTURES=$2

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
	(cd ${distribution}
	    section=main
	    for architecture in ${ARCHITECTURES}; do
		mkdir -p dists/${target}/${section}/binary-${architecture}
	    done
	    apt-ftparchive generate generate-${target}.conf
	    rm -f dists/${target}/Release*
	    apt-ftparchive -c release-${target}.conf \
		release dists/${target} > /tmp/Release
	    mv /tmp/Release dists/${target}
	);
    done;
done
