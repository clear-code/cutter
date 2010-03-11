#!/bin/zsh

script_base_dir=`dirname $0`

run()
{
    "$@"
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

for rpm in $script_base_dir/fedora/12/*/*/*.rpm; do
    run rpm -D "_gpg_name `$script_base_dir/gpg-uid.sh`" --resign $rpm
done
