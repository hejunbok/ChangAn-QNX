TEMPLATE = subdirs

SUBDIRS += kanzi qpps qplayer qtqnxcar2 qtqnxcar2-ui app keyboard settingsapp

CONFIG += ordered

# run the doxygen command in source folder, output will be generated there
docs.target = docs
docs.commands = (cd $$PWD; doxygen Doxyfile)
docs.depends = FORCE
QMAKE_EXTRA_TARGETS += docs

SUBDIRS += tests

# test target
# QMakeTestRunner seems to be broken on Windows...
win32 {
    test.target = test
    test.commands = $$PWD/runtests.bat $$OUT_PWD
    test.depends = all
    QMAKE_EXTRA_TARGETS += test
} else:qnx {
    # When cross-compiling for QNX we still want to have the tests compiled
    # But we don't want to run them, obviously
    test.target = test
    test.depends = all
    QMAKE_EXTRA_TARGETS += test
} else {
    TEST_LIB_PATHS += $$OUT_PWD/lib
    include(3rdparty/QMakeTestRunner/testtarget.pri)
}
