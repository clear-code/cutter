#!/bin/sh
# -*- sh-indentation: 2; sh-basic-offset: 2 -*-
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

LANG=C

set -u

run()
{
  "$@"
  if test $? -ne 0; then
    echo "Failed $@"
    exit 1
  fi
}

. /host/env.sh

distribution=$(lsb_release --id --short | tr 'A-Z' 'a-z')
code_name=$(lsb_release --codename --short)
case "${distribution}" in
  debian)
    component=main
    ;;
  ubuntu)
    component=universe
    ;;
esac
architecture=$(dpkg-architecture -q DEB_BUILD_ARCH)

run mkdir -p build
run cp /host/tmp/${PACKAGE}-${VERSION}.tar.gz \
  build/${PACKAGE}_${VERSION}.orig.tar.gz
run cd build
run tar xfz ${PACKAGE}_${VERSION}.orig.tar.gz
case "${VERSION}" in
  *~dev*)
    run mv ${PACKAGE}-$(echo $VERSION | sed -e 's/~dev/-dev/') \
        ${PACKAGE}-${VERSION}
    ;;
  *~rc*)
    run mv ${PACKAGE}-$(echo $VERSION | sed -r -e 's/~rc[0-9]+//') \
        ${PACKAGE}-${VERSION}
    ;;
esac
run cd ${PACKAGE}-${VERSION}/
platform="${distribution}-${code_name}"
if [ -d "/host/tmp/debian.${platform}-${architecture}" ]; then
  run cp -rp "/host/tmp/debian.${platform}-${architecture}" debian
elif [ -d "/host/tmp/debian.${platform}" ]; then
  run cp -rp "/host/tmp/debian.${platform}" debian
else
  run cp -rp "/host/tmp/debian" debian
fi
DEB_BUILD_OPTIONS="parallel=$(nproc)"
# DEB_BUILD_OPTIONS="${DEB_BUILD_OPTIONS} noopt"
export DEB_BUILD_OPTIONS
if [ "${DEBUG:-no}" = "yes" ]; then
  run debuild -us -uc
else
  run debuild -us -uc > /dev/null
fi
run cd -

repositories="/host/repositories"
package_initial=$(echo "${PACKAGE}" | sed -e 's/\(.\).*/\1/')
pool_dir="${repositories}/${distribution}/pool/${code_name}/${component}/${package_initial}/${PACKAGE}"
run mkdir -p "${pool_dir}/"
run cp \
  *.*deb \
  *.dsc \
  *.tar.* \
  "${pool_dir}/"

run chown -R "$(stat --format "%u:%g" "${repositories}")" "${repositories}"
