include(common-vars.pri)
include(common-project-config.pri)

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
    po \
    lib \
    src \
    plugins \
    tests

include(common-installs-config.pri)

desktop.path = $$PREFIX/share/applications
desktop.files += ubuntu-system-settings.desktop

INSTALLS += desktop

DISTNAME = $${PROJECT_NAME}-$${PROJECT_VERSION}
dist.commands = "bzr export $${DISTNAME}.tar.bz2"
QMAKE_EXTRA_TARGETS += dist
