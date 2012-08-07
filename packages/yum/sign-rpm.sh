#!/bin/sh

script_base_dir=`dirname $0`

if [ $# != 1 ]; then
    echo "Usage: $0 DISTRIBUTIONS"
    echo " e.g.: $0 'fedora centos'"
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
    run rpm -D "_gpg_name ${GPG_UID}" \
	-D "_gpg_digest_algo sha1" \
	-D "__gpg /usr/bin/gpg2" \
	-D "__gpg_check_password_cmd /bin/true true" \
	-D "__gpg_sign_cmd %{__gpg} gpg --batch --no-verbose --no-armor %{?_gpg_digest_algo:--digest-algo %{_gpg_digest_algo}} --no-secmem-warning -u \"%{_gpg_name}\" -sbo %{__signature_filename} %{__plaintext_filename}" \
	--resign $script_base_dir/${distribution}/*/*/*/*.rpm
done
