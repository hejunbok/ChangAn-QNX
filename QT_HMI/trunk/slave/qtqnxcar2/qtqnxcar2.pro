TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = qtqnxcar2
!qnx {
    SUBDIRS += tests
    tests.depends += qtqnxcar2
}

docs.target = docs
docs.commands = cd $$PWD && doxygen $$PWD/Doxyfile
QMAKE_EXTRA_TARGETS += docs
