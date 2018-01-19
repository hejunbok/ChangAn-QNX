# QMake .pri file that provides a "make test" target that can be configured from the project file.
#
# This file is part of QMakeTestRunner - Python Test Runner for Qt and QMake.
#
# (C) Agile Workers Software, 2012.
# Author: Mirko Boehm <mirko@agile-workers.com>
#
# Licensed under the GPL v3.

# Configuration variables:
# TEST_LIB_PATHS: list of paths to be added to the dynamic linker search path, may be empty
# TEST_VERBOSE: if not empty, enables additional debug output from the test runner

# Fewer customer serviceable parts after this line...:
#
# Check if python exists and is callable:
PythonOutputString=$$system(python -c "print\\(\\'QMakeTestRunner\\'\\);")
# message($$PythonOutputString)
contains(PythonOutputString, "QMakeTestRunner") {
    # all good, Python detected
} else {
    error("Python not found. The Python QMake Test Runner requires Python.")
}

# The test runner is supposed to be located next to the .pri file:
TEST_RUNNER="TESTRUNNER=\"python $$PWD/testrunner.py\""

# Configure TEST_ARGS to contain the parameters for the test runner:
TEST_LIB_PATH_ARGS=''
for(TEST_LIB_PATH_ENTRY, TEST_LIB_PATHS) : TEST_LIB_PATH_ARGS += "--lib-path $${TEST_LIB_PATH_ENTRY}"
TEST_FRAMEWORK_PATH_ARGS=''
for(TEST_FRAMEWORK_PATH_ENTRY, TEST_FRAMEWORK_PATHS) : TEST_FRAMEWORK_PATH_ARGS += "--framework-path $${TEST_FRAMEWORK_PATH_ENTRY}"
# Verbose?
!isEmpty(TEST_VERBOSE):TEST_VERBOSE_TEXT="--verbose"

TEST_ARGS="TESTARGS=\"$${TEST_LIB_PATH_ARGS} $${TEST_FRAMEWORK_PATH_ARGS} $${TEST_VERBOSE_TEXT}\""

# add "make test" make target, use it to configure make check:
test.target = test
unix:test.commands = make check $${TEST_RUNNER} $${TEST_ARGS}
win32:test.commands = nmake check $${TEST_RUNNER} $${TEST_ARGS}
test.depends = all $(TARGET)
QMAKE_EXTRA_TARGETS += test
