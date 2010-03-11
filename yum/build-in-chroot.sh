#!/bin/sh

if [ $# != 3 ]; then
    echo "Usage: $0 VERSION CHROOT_BASE ARCHITECTURES"
    echo " e.g.: $0 1.1.1 /var/lib/chroot 'i386 x86_64'"
    exit 1
fi

VERSION=$1
CHROOT_BASE=$2
ARCHITECTURES=$3
CODES=fedora-12
PACKAGE=cutter

PATH=/usr/local/sbin:/usr/sbin:$PATH

script_base_dir=`dirname $0`

run()
{
    "$@"
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

run_sudo()
{
    run sudo "$@"
}

build_chroot()
{
    architecture=$1
    code_name=$2

    if [ $architecture = "x86_64" ]; then
	rinse_architecture="amd64"
        fedora_architecture=$architecture
    else
	rinse_architecture=$architecture
        fedora_architecture=i686
    fi

    run_sudo mkdir -p ${base_dir}/etc/rpm
    run_sudo sh -c "echo ${fedora_architecture}-fedora-linux > ${base_dir}/etc/rpm/platform"
    run_sudo rinse \
	--arch $rinse_architecture \
	--distribution $code_name \
	--directory $base_dir
    run_sudo rinse --arch $rinse_architecture --clean-cache

    run_sudo sh -c "echo >> /etc/fstab"
    run_sudo sh -c "echo /dev ${base_dir}/dev none bind 0 0 >> /etc/fstab"
    run_sudo sh -c "echo devpts-chroot ${base_dir}/dev/pts devpts defaults 0 0 >> /etc/fstab"
    run_sudo sh -c "echo proc-chroot ${base_dir}/proc proc defaults 0 0 >> /etc/fstab"
    run_sudo mount ${base_dir}/dev
    run_sudo mount ${base_dir}/dev/pts
    run_sudo mount ${base_dir}/proc
}

build()
{
    architecture=$1
    code_name=$2

    target=${code_name}-${architecture}
    base_dir=${CHROOT_BASE}/${target}
    if [ ! -d $base_dir ]; then
	run build_chroot $architecture $code_name
    fi

    source_dir=${script_base_dir}/..
    build_user=${PACKAGE}-build
    build_user_dir=${base_dir}/home/${build_user}
    rpm_base_dir=${build_user_dir}/rpm
    rpm_dir=${rpm_base_dir}/RPMS/${architecture}
    srpm_dir=${rpm_base_dir}/SRPMS
    pool_base_dir=fedora/12
    binary_pool_dir=$pool_base_dir/$architecture/Packages
    source_pool_dir=$pool_base_dir/source/SRPMS
    run cp $source_dir/${PACKAGE}-${VERSION}.tar.gz \
	${CHROOT_BASE}/$target/tmp/
    run cp $source_dir/rpm/fedora/${PACKAGE}.spec ${CHROOT_BASE}/$target/tmp/
    run echo $PACKAGE > ${CHROOT_BASE}/$target/tmp/build-package
    run echo $VERSION > ${CHROOT_BASE}/$target/tmp/build-version
    run echo $build_user > ${CHROOT_BASE}/$target/tmp/build-user
    run cp ${script_base_dir}/${PACKAGE}-depended-packages \
	${CHROOT_BASE}/$target/tmp/depended-packages
    run cp ${script_base_dir}/build-rpm.sh \
	${CHROOT_BASE}/$target/tmp/
    run_sudo rm -rf $rpm_dir $srpm_dir
    run_sudo su -c "chroot ${CHROOT_BASE}/$target /tmp/build-rpm.sh"
    run mkdir -p $binary_pool_dir
    run mkdir -p $source_pool_dir
    run cp -p $rpm_dir/*-${VERSION}* $binary_pool_dir
    run cp -p $srpm_dir/*-${VERSION}* $source_pool_dir
}

for architecture in $ARCHITECTURES; do
    for code_name in $CODES; do
	build $architecture $code_name
    done;
done
