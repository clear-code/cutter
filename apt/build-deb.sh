#!/bin/sh

VERSION=1.1.0

sudo aptitude -V -D update && sudo aptitude -V -D -y safe-upgrade
sudo aptitude install -y subversion devscripts debhelper cdbs autotools-dev \
    intltool gtk-doc-tools libgtk2.0-dev libgoffice-0-{6,8}-dev \
    libgstreamer0.10-dev libsoup2.4-dev

mkdir -p ~/work/c
if [ -d ~/work/c/cutter ]; then
    cd ~/work/c/cutter
    svn up
else
    cd ~/work/c
    svn co https://cutter.svn.sourceforge.net/svnroot/cutter/cutter/trunk cutter
fi

cd ~/work/c
rm -rf cutter-${VERSION}
tar xfz cutter_${VERSION}.orig.tar.gz
cd cutter-${VERSION}

mkdir debian
cp -rp ../cutter/debian/* debian/

if dpkg -l libgoffice-0-8-dev > /dev/null 2>&1; then
    :
else
    sed -i'' -e 's/libgoffice-0-8/libgoffice-0-6/g' debian/control
fi

debuild -us -uc
