TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = qtqnxcar2-ui
!qnx {
    SUBDIRS += tests
    tests.depends += qtqnxcar2-ui
}

docs.target = docs
docs.commands = cd $$PWD && doxygen $$PWD/Doxyfile
QMAKE_EXTRA_TARGETS += docs
