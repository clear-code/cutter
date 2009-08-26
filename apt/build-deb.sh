#!/bin/sh

VERSION=1.0.8

sudo aptitude update
sudo aptitude install -y subversion devscripts debhelper cdbs autotools-dev \
    intltool gtk-doc-tools libgtk2.0-dev libgoffice-0-{6,8}-dev \
    libgstreamer0.10-dev

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

debuild -us -uc
