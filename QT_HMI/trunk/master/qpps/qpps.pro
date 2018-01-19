TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = qpps
!win32: SUBDIRS += monitor

SUBDIRS += tests

tests.depends += qpps

docs.target = docs
docs.commands = cd $$PWD && doxygen $$PWD/Doxyfile
QMAKE_EXTRA_TARGETS += docs
