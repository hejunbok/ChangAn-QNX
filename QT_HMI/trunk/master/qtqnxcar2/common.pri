# absolute path to project source root
SOURCE_ROOT = $$PWD

# relative path of the includer
includerRelPath = $$_PRO_FILE_PWD_
includerRelPath ~= s,^$$re_escape($$SOURCE_ROOT/),,

# absolute path to project build root
BUILD_ROOT = $$OUT_PWD
BUILD_ROOT ~= s,$$re_escape($$includerRelPath)$,,

PROROOT = $$SOURCE_ROOT
win32: PROLIB = $$BUILD_ROOT/bin
else: PROLIB = $$BUILD_ROOT/lib
PROTEST = $$BUILD_ROOT/tests

# overwrite variables from parent ../common.pri file in case it exists
exists($$PWD/../common.pri): include($$PWD/../common.pri)

DEFAULT_INSTALL_PREFIX=/usr
isEmpty(INSTALL_PREFIX): INSTALL_PREFIX=$$DEFAULT_INSTALL_PREFIX
