#!/bin/sh

set -e

distribution=$(lsb_release --short --id | tr 'A-Z' 'a-z')
case $distribution in
    debian)
	code_name=$(lsb_release --short --codename)
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
