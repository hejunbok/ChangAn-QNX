# QMakeTestRunner

QMakeTestRunner assists in setting library paths when running unit tests in products built with QMake. It makes _make test_ work out of the box right after makefiles have been generated. It is Free Software, published on Github, and small and easy to integrate into projects as a Git submodule.

## Motivation, aka "The Problem"

QMake as a build system is simple and complete for building code. A repeated problem we run into when using QMake is that it does not implement features for _executing_ parts of the project (like tests). Others ran into this issue as well, as for example this discussion of specifying [run configurations from QMake on Qt DevNet](https://qt-project.org/forums/viewthread/15717) indicates. Multi-part projects often consist of dynamic libraries that contain the features, and applications and tests that link this library. Amongst other things, this approach allows tests to link objects which contain the application's code. Now when the tests are executed, the dynamic linker needs to be able to pick up the freshly built library and link the test with it at execution time. Projects usually run test cases with _make test_, and ideally the tests should execute out of the box after the sources have been configured and built (out of the box meaning that no tweaking of the environment variables should be needed between make and make test). After all, it should be as easy as possible for developers to execute tests. To solve this problem, paths that are created in the build directory need to be added to the dynamic linker library path before the tests can be executed. Especially with shadow builds, this would require manual setup, and _make test_ would not work out of the box after QMake has been run.

This is a problem in IDEs like Qt Creator, but also others - for every build configuration (having separate shadow build directories), the developer needs to configure the library path before being able to debug the application. This second problem needs to be fixed in the IDEs. For the first one - configuring test runs from the QMake .pro file - we have developed _QMakeTestRunner_.

## QMakeTestRunner, aka "The Solution"

As soon as any target in a QMake project has the _testcase_ attribute assigned to *CONFIG*, QMake automatically generates a target called _check_ that runs this test. Unfortunately, _make check_ does not allow the manipulation of the linker paths before running the tests either. QMakeTestRunner contains boilerplate code that wraps QMake's _make check_ target with a new one called _make test_, and through variables specified in the QMake _.pro_ file, allows to specify paths to be added to the linker path.

## Usage

QMakeTestRunner is intended to be used as a git submodule to the main project. It should not require any modifications to be used. It requires Python to be in the path, checks for it, and the QMake run will fail if Python cannot be detected. When using other version control systems than Git, QMakeTestRunner should be small enough to simply be copied into the project as a subdirectory.

### Project configuration example

The following example adds the directory _libs/_ in the project build directory to the linker path. It assumes the QMakeTestRunner repository is located under _3rdparty/QMakeTestRunner_. The path where the dynamic library is generated in is located below the output (build) directory, which is why it is prefixed with *OUT_PWD*:

<pre><code>TEST_LIB_PATHS += $$OUT_PWD/libs
include( 3rdparty/QMakeTestRunner/testtarget.pri )
</code></pre>

To trigger extra diagnostic output of the test runner, add 

<pre><code>TEST_VERBOSE = 1
</code></pre>

before including _testtarget.pri_.

### _make test_

Once _testtarget.pri_ has been included, a _test_ target is defined in the makefiles. Simply run _make test_ to execute the original _make check_ with the necessary paths.

### Mac OSX Frameworks

Frameworks on OSX are a different kind of library. The framework path is passed in to the dynamic linker using a different environment variable. To specify a framework path, set the _TEST_FRAMEWORK_PATHS_ variable before including _testtarget.pri_.

<pre><code>TEST_FRAMEWORK_PATHS +=  $$OUT_PWD/frameworks</pre></code>

###Setting up QMakeTestRunner as a Git submodule

Using a Git submodule has the benefit that the code for the runner does not need to be duplicated into the repository of the project that is supposed to use it. It also makes it easy to pull updates to the test runner by simply updating the submodule to a new revision. To add the submodule as _3rdparty/QMakeTestRunner_ in your project, follow these steps:
<pre><code>> mkdir -p 3rdparty
> git submodule add git://github.com/AgileWorkersSoftware/QMakeTestRunner.git 3rdparty/QMakeTestRunner
> git submodule init</code></pre>

The directory _3rdparty/QMakeTestRunner_ should now contain the current revision of the test runner scripts.

# License, contributions, issues and support

_QMakeTestRunner_ is Free Software licensed under the GPL, version 3. Contributions to it are welcome, please propose them as Github pull requests. To submit a bug report or feature wish, please use the Github issue tracker for the project. 

Commercial support for QMaketestRunner or general issues with QMake is provided by [Agile Workers Software](https://github.com/AgileWorkersSoftware), we are happy to help.
