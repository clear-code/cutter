#!/bin/sh

LANG=C

PACKAGE=$(cat /tmp/build-package)
USER_NAME=$(cat /tmp/build-user)
VERSION=$(cat /tmp/build-version)
DEPENDED_PACKAGES=$(cat /tmp/depended-packages)
BUILD_SCRIPT=/tmp/build-deb-in-chroot.sh
CODE_NAME=$(lsb_release -s -c)
GSTREAMER_INSTALL=gstreamer0.10-plugins-cutter.install

run()
{
    "$@"
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

if [ ! -x /usr/bin/aptitude ]; then
    run apt-get update
    run apt-get install -y aptitude
fi
run aptitude update -V -D
run aptitude safe-upgrade -V -D -y

if ! aptitude show libgoffice-0.8-dev > /dev/null 2>&1; then
    DEPENDED_PACKAGES=$(echo $DEPENDED_PACKAGES | sed -e 's/libgoffice-0.8-dev//')
fi

run aptitude install -V -D -y devscripts ${DEPENDED_PACKAGES}
run aptitude clean

if ! id $USER_NAME >/dev/null 2>&1; then
    run useradd -m $USER_NAME
fi

if test "${PACKAGE}" = "cutter"; then
    ARCHITECTURE=$(dpkg --print-architecture)
    if test ${ARCHITECTURE} = "amd64"; then
        ARCHITECTURE="x86_64";
    fi
    if [ -d "/usr/lib/${ARCHITECTURE}-linux-gnu/gstreamer-0.10" ]; then
        LIB_ARCHITECTURE="${ARCHITECTURE}-linux-gnu"
        sed -i'' -e "s/usr\/lib/usr\/lib\/${LIB_ARCHITECTURE}/" \
            /tmp/${PACKAGE}-debian/${GSTREAMER_INSTALL}
    fi
fi

cat <<EOF > $BUILD_SCRIPT
#!/bin/sh

rm -rf build
mkdir -p build

cp /tmp/${PACKAGE}-${VERSION}.tar.gz build/${PACKAGE}_${VERSION}.orig.tar.gz
cd build
tar xfz ${PACKAGE}_${VERSION}.orig.tar.gz
cd ${PACKAGE}-${VERSION}/
cp -rp /tmp/${PACKAGE}-debian debian
if ! dpkg -l libgoffice-0.8-dev > /dev/null 2>&1; then
    mv debian/control debian/control.tmp
    grep -v libgoffice-0.8-dev debian/control.tmp > debian/control
    rm debian/control.tmp
fi
debuild -us -uc
EOF

run chmod +x $BUILD_SCRIPT
run su - $USER_NAME $BUILD_SCRIPT
