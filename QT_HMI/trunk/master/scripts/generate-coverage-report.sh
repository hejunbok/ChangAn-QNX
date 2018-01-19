#!/bin/bash

# This script can be used to generate LCOV/GCOV data
# It is recommended to call this script out-of-source

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SOURCE_DIR=$SCRIPT_DIR/..

# exit immediately if a command returns with non-zero status
set -e

qmake -r 'QMAKE_CXXFLAGS+="--coverage -fprofile-arcs -ftest-coverage"' 'QMAKE_CFLAGS+="--coverage -fprofile-arcs -ftest-coverage"' 'QMAKE_LFLAGS+="--coverage -fprofile-arcs -ftest-coverage -lgcov"' CONFIG+=release CONFIG-=debug $SOURCE_DIR
make
make test

lcov -c -d . -o gcov.info

# get rid off build files
lcov --remove gcov.info '*_moc*' '*moc_*' 'qrc*' '*include*' '*3rdparty*' 'build/*' -d . -o gcov.info

# try to get rid off Qt5 files when built in developer-mode
lcov --remove gcov.info 'qtbase/*' -d . -o gcov.info

# get rid off non-interesting parts of the project
lcov --remove gcov.info -d . -o gcov.info

genhtml -o coverage gcov.info
