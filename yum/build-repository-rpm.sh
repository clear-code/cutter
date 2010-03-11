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

PACKAGE=cutter
rpm_base_dir=$HOME/rpm

if [ ! -f ~/.rpmmacros ]; then
    run cat <<EOM > ~/.rpmmacros
%_topdir $rpm_base_dir
EOM
fi

run mkdir -p $rpm_base_dir/SOURCES
run mkdir -p $rpm_base_dir/SPECS
run mkdir -p $rpm_base_dir/BUILD
run mkdir -p $rpm_base_dir/RPMS
run mkdir -p $rpm_base_dir/SRPMS

run tar cfz $rpm_base_dir/SOURCES/${PACKAGE}-repository.tar.gz \
  -C ${script_base_dir} ${PACKAGE}.repo RPM-GPG-KEY-${PACKAGE}
run cp ${script_base_dir}/${PACKAGE}-repository.spec $rpm_base_dir/SPECS/

run rpmbuild -ba $rpm_base_dir/SPECS/${PACKAGE}-repository.spec

top_dir=$script_base_dir/fedora

run mkdir -p $top_dir
run cp -p $rpm_base_dir/RPMS/noarch/${PACKAGE}-repository-* $top_dir
run cp -p $rpm_base_dir/SRPMS/${PACKAGE}-repository-* $top_dir

run cp -p ${script_base_dir}/RPM-GPG-KEY-${PACKAGE} $top_dir
