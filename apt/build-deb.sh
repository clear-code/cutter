#!/bin/sh

VERSION=1.0.8

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
cp -rp ../cutter/debian/* ./

if which debuild > /dev/null; then
    :
else
    sudo aptitude update
    sudo aptitude install -y devscripts
fi

debuild -us -uc
