#!/bin/sh

set -e

if [ "$CUTTER_MASTER" = "yes" ] || [ "$CUTTER_MAIN" = "yes" ] ; then
    if type apt > /dev/null 2>&1; then
        sudo apt update -qq
        sudo apt install -qq -y \
             autotools-dev \
             intltool \
             libsoup2.4-dev
        git clone --depth 1 https://github.com/clear-code/cutter.git
        cd cutter
        ./autogen.sh
        ./configure --prefix=/usr --localstatedir=/var --enable-debug
        make -j > /dev/null
        sudo make install > /dev/null
        cd ..
    else
        case $(. /etc/os-release && echo "${VERSION_ID}") in
            8.*)
                additional_repository=powertools
                ;;
            *)
                additional_repository=crb
                ;;
        esac
        sudo dnf install --enablerepo=${additional_repository} -y \
             autoconf \
             automake \
             diffutils \
             gcc \
             gcc-c++ \
             git \
             glib2-devel \
             gtk-doc \
             intltool \
             libtool \
             make
        git clone --depth 1 https://github.com/clear-code/cutter.git
        cd cutter
        ./autogen.sh
        ./configure --prefix=/usr --localstatedir=/var --enable-debug
        make -j > /dev/null
        sudo make install > /dev/null
        cd ..
    fi
else
    distribution=$(lsb_release --short --id | tr 'A-Z' 'a-z')
    case $distribution in
        debian)
	    code_name=$(lsb_release --short --codename)
            if [ "x${CODE_NAME}" != "x" ]; then
                code_name=$CODE_NAME
            fi
	    component=main
	    apt_url_base=https://osdn.net/projects/cutter/storage
	    cat <<EOF | sudo tee /etc/apt/sources.list.d/cutter.list
deb ${apt_url_base}/${distribution}/ ${code_name} ${component}
deb-src ${apt_url_base}/${distribution}/ ${code_name} ${component}
EOF
	    sudo apt update -qq
	    sudo apt install -qq -y --allow-unauthenticated cutter-keyring
	    ;;
        ubuntu)
	    sudo apt install -qq -y -V software-properties-common
	    sudo add-apt-repository -y ppa:cutter-testing-framework/ppa
	    ;;
    esac

    sudo apt update -qq
    sudo apt install -qq -y -V cutter-testing-framework
fi
