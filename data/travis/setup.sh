#!/bin/sh

set -e

if [ "$CUTTER_MASTER" = "yes" ]; then
    sudo apt-get update -qq
    sudo apt-get install -qq -y \
         autotools-dev \
         intltool \
         libgtk2.0-dev \
         libgoffice-0.8-dev \
         libsoup2.4-dev
    git clone --depth 1 --branch master https://github.com/clear-code/cutter.git
    cd cutter
    ./autogen.sh
    ./configure --prefix=/usr --localstatedir=/var --enable-debug
    make -j > /dev/null
    sudo make install > /dev/null
    cd ..
else
distribution=$(lsb_release --short --id | tr 'A-Z' 'a-z')
case $distribution in
    debian)
	code_name=$(lsb_release --short --codename)
        if [ "x${CODE_NAME}" != "x" ]; then
            code_name=$CODE_NAME
        fi
	component=main
	apt_url_base=https://sourceforge.net/projects/cutter/files
	cat <<EOF | sudo tee /etc/apt/sources.list.d/cutter.list
deb ${apt_url_base}/${distribution}/ ${code_name} ${component}
deb-src ${apt_url_base}/${distribution}/ ${code_name} ${component}
EOF
	sudo apt-get update -qq
	sudo apt-get install -qq -y --allow-unauthenticated cutter-keyring
	;;
    ubuntu)
	sudo apt-get install -qq -y -V software-properties-common
	sudo add-apt-repository -y ppa:cutter-testing-framework/ppa
	;;
esac

sudo apt-get update -qq
sudo apt-get install -qq -y -V cutter-testing-framework
fi
