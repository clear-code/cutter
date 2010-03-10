#!/bin/sh

script_base_dir=`dirname $0`

run()
{
    "$@"
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

for dir in $script_base_dir/fedora/12/*; do
    run createrepo $dir
done

run $script_base_dir/gpg-public-key.sh > \
    $script_base_dir/fedora/RPM-GPG-KEY-cutter
