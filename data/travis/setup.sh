#!/bin/sh

set -e
set -x

distribution=$(lsb_release --short --id | tr 'A-Z' 'a-z')
code_name=$(lsb_release --short --codename)
component=main
apt_url_base=http://downloads.sourceforge.net/project/cutter
cat <<EOF | sudo tee /etc/apt/sources.list.d/groonga.list
deb ${apt_url_base}/${distribution}/ ${code_name} ${component}
deb-src ${apt_url_base}/${distribution}/ ${code_name} ${component}
EOF

sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 1C837F31
sudo apt-get update
sudo apt-get -y install cutter-testing-framework
