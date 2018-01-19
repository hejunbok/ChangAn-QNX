#!/usr/bin/env python

import os, shutil, subprocess, sys
from distutils import dir_util
from shared import run, create_empty

def append_to_file(outfn, line):
    out = open(outfn, 'a+')
    try:
        out.write(line)
    finally:
        out.close()

class Options:
    def __init__(self):
        if sys.platform.startswith('win32'):
            home = os.environ['HOMEPATH']
        else:
            home = os.environ['HOME']
        self.home = home
        # default config start

        self.qt5_srcbase = os.path.join(home, 'source', 'qt5')
        self.qt5_buildbase = None
        self.qt5_installbase = os.path.join(home, 'Qt5')
        if sys.platform.startswith('darwin') or sys.platform.startswith('win32'):
            self.buildmode = None
        else:
            self.buildmode = 'release'
        self.qt5_version = 'master' # this is just for install paths etc., it doesn't change branches etc.
        self.baseConfigureFlags = '-opensource -confirm-license -opengl desktop -icu -nomake demos -nomake examples -nomake docs'
        self.xplatform = None
        self.target_makeflags = ""
        self.qt5_modules = 'qtbase qtxmlpatterns qtjsbackend qtdeclarative qtscript qtjsondb qtsvg qt3d qtlocation qtquick1 qtstyleplugins'
        if sys.platform.startswith('darwin'):
            self.platform = 'macx-clang'
        else:
            self.platform = None

def get_svn_revision(srcdir):
    p = subprocess.Popen(['svnversion'], cwd=srcdir, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    if p.returncode != 0:
        raise Exception('"svnversion" failed with exit code %s' % (str(p.returncode)))
    return stdout

def get_git_revision(srcdir):
    p = subprocess.Popen(['git', 'rev-parse', 'HEAD'], cwd=srcdir, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    if p.returncode != 0:
        raise Exception('"git rev-parse HEAD" failed with exit code %s' % (str(p.returncode)))
    return stdout

def write_to_revisions_file(installTarget, line):
    outfn = os.path.join(installTarget, 'REVISIONS')
    dir_util.mkpath(installTarget)
    append_to_file(outfn, line)

def write_scm_revision(srcbase, installTarget, module):
    if windows:
        return
    srcdir = os.path.join(srcbase, module)
    # First try git, if that fails, try svn (for non-qt modules built with this script)
    try:
        revision = get_git_revision(srcdir)
    except:
        revision = get_svn_revision(srcdir)
    write_to_revisions_file(installTarget, '%s: %s' % (module, revision))

def makeInstall(cwd):
    if osx: # parallel make install seems fragile on OS X
        run(make, ['-j1', 'install'], cwd=cwd)
    else:
        run(make, ['install'], cwd=cwd)

def build_qtmodule(srcbase, buildbase, module, buildmode, installTarget):
    srcdir = os.path.join(srcbase, module)
    builddir = os.path.join(buildbase, module)
    create_empty(builddir)

    if buildmode == 'debug':
        configOptions = ['CONFIG-=release', 'CONFIG+=debug', 'CONFIG-=debug_and_release']
    elif buildmode == 'release':
        configOptions = ['CONFIG+=release', 'CONFIG-=debug', 'CONFIG-=debug_and_release']
    else:
        configOptions = ['CONFIG-=release', 'CONFIG-=debug', 'CONFIG+=debug_and_release']
    run(os.path.join(installTarget, 'bin', 'qmake'), configOptions + [srcdir], cwd=builddir)
    run(make, cwd=builddir)
    makeInstall(cwd=builddir)

def build_qtbase(srcbase, buildbase, configureFlags, target_makeflags):
    srcdir = os.path.join(srcbase, 'qtbase')
    if osx: # out-of-source doesn't work on OS X (August 7 2012)
        builddir = srcdir
        run('git', ['clean', '-xdf'], cwd=builddir)
    else:
        builddir = os.path.join(buildbase, 'qtbase')
        create_empty(builddir)
    configure = 'configure.bat' if windows else 'configure'
    absConfigure = os.path.join(srcdir, configure)
    run(absConfigure, configureFlags, cwd=builddir)
    if target_makeflags != "":
        run(make, ['sub-src-qmake_all'], cwd=builddir)
        run(make, cwd=os.path.join(builddir, 'src', 'tools'))
        os.environ['MAKEFLAGS'] = os.environ['MAKEFLAGS'] + ' ' + target_makeflags
    run(make, cwd=builddir)
    makeInstall(cwd=builddir)

def mode_to_key(buildmode):
    # Windows/MSVC fails linking when "debug-and-release" is in the build dir path.
    # There seems to be some magic in the toolchain that goes bonkers, so work around
    # it by avoiding 'debug' and 'release' in the path.
    if windows and buildmode == 'debug-and-release':
        return 'dbg-and-rel'
    else:
        return buildmode

def make_buildkey(o):
    global windows
    windows = not o.xplatform and sys.platform.startswith('win32')
    return "-".join(filter(None, [o.qt5_version, o.xplatform.replace(os.path.sep, '_') if o.xplatform else 'desktop', mode_to_key(o.buildmode)]))

def build_qt5(prefix, options=Options()):
    o = options # for brevity
    # Source local.py if it exists, to override defaults

    global windows
    windows = not o.xplatform and sys.platform.startswith('win32')
    global osx
    osx = not o.xplatform and sys.platform.startswith('darwin')
    global make
    make = 'make' if not windows else 'nmake' # TODO handle mingw-make, nmake etc.

    buildkey = make_buildkey(o)
    if prefix:
        installTarget = prefix
    else:
        installTarget = os.path.join(o.qt5_installbase, buildkey)

    write_to_revisions_file(installTarget, 'buildkey: %s\n' % buildkey)

    compileBuild = None
    if options.qt5_buildbase:
        compileBuild = os.path.join(o.qt5_buildbase, 'build-' + buildkey)
    else:
        compileBuild = os.path.join(o.qt5_srcbase, 'build-' + buildkey)

    if windows:
        configureFlagsPS = ''
    elif osx:
        configureFlagsPS = '-L/usr/local/opt/icu4c/lib -I/usr/local/opt/icu4c/include' # assuming ICU was installed via homebrew (brew install icu4c)
    else:
        configureFlagsPS = '-xcb'

    buildmodeFlag = "-" + o.buildmode if o.buildmode else ''
    configureFlags = (configureFlagsPS + ' ' + o.baseConfigureFlags).split()
    configureFlags += ['-prefix', installTarget]
    if o.xplatform:
        configureFlags += ['-xplatform', o.xplatform]
    if o.platform:
	configureFlags += ['-platform', o.platform]
    if o.buildmode:
        configureFlags.append('-' + o.buildmode)

    for module in o.qt5_modules.split():
        write_scm_revision(o.qt5_srcbase, installTarget, module)
        if module.startswith('qtbase'):
            build_qtbase(o.qt5_srcbase, compileBuild, configureFlags, o.target_makeflags)
        else:
            build_qtmodule(o.qt5_srcbase, compileBuild, module, o.buildmode, installTarget)

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='build Qt5')
    parser.add_argument('--prefix', dest='prefix')
    args = parser.parse_args()
    build_qt5(args.prefix)
