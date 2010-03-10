#!/bin/sh

PACKAGE=cutter
USER_NAME=${PACKAGE}-build
BUILD_SCRIPT=/tmp/build-${PACKAGE}.sh
VERSION=`cat /tmp/${PACKAGE}-version`

yum update -y
yum install -y rpm-build \
    intltool gettext gtk-doc gcc gcc-c++ make glib2-devel libsoup-devel
yum clean packages

if ! id $USER_NAME >/dev/null 2>&1; then
    useradd -m $USER_NAME
fi

cat <<EOF > $BUILD_SCRIPT
#!/bin/sh

if [ ! -f ~/.rpmmacros ]; then
    cat <<EOM > ~/.rpmmacros
%_topdir \$HOME/rpm
EOM
fi

mkdir -p rpm/SOURCES
mkdir -p rpm/SPECS
mkdir -p rpm/BUILD
mkdir -p rpm/RPMS
mkdir -p rpm/SRPMS

cp /tmp/${PACKAGE}-$VERSION.tar.gz rpm/SOURCES/
cp /tmp/${PACKAGE}.spec rpm/SPECS/

chmod o+rx . rpm rpm/RPMS rpm/SRPMS

rpmbuild -ba rpm/SPECS/${PACKAGE}.spec
EOF

chmod +x $BUILD_SCRIPT
su - $USER_NAME $BUILD_SCRIPT
