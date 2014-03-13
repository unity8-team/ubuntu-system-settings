#!/bin/sh

TOPDIR=$(readlink -e "$(dirname ${0})/..")

LOCAL_PRIVATE_DIR=$(ls -d ${TOPDIR}/debian/tmp/usr/lib/*/ubuntu-system-settings/private)
if [ -n ${LOCAL_PRIVATE_DIR} ]; then
    echo "Testing against locally built version"
    export QML2_IMPORT_PATH=${LOCAL_PRIVATE_DIR}:${QML2_IMPORT_PATH}
    export PATH=${TOPDIR}/debian/tmp/usr/bin:${PATH}
else
    echo "Testing against system version"
fi

export QML2_IMPORT_PATH=${TOPDIR}/tests/mocks:${QML2_IMPORT_PATH}

system-settings-wizard
