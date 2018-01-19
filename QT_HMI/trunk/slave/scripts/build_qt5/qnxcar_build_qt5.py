#!/usr/bin/env python

import argparse, os, subprocess, sys, tarfile
from distutils import dir_util
from shared import run
import build_qt5

def untar(tarball, dst):
    archive = tarfile.open(tarball)
    try:
        archive.extractall(dst)
    finally:
        archive.close()

def write_module_revision(installTarget, module, revision):
    line = '%s: %s\n' % (module, revision)
    build_qt5.write_to_revisions_file(installTarget, line)

def write_sdp_revision(installTarget, versionfile):
    dir_util.mkpath(installTarget)
    in_file = open(versionfile, 'r')
    try:
        line = in_file.read() + '\n'
        build_qt5.write_to_revisions_file(installTarget, line)
    finally:
        in_file.close()

def ensure_sdp_sourced():
    subprocess.check_call(['qcc', '-V'])

def qnx_build_qt5(source_zip = None, sources_only = False, options=build_qt5.Options()):
    if source_zip:
        modules = [i for i in options.qt5_modules.split() if i not in ['QNXStartupDemo']]
        package_qt5_sources.package_qt5_sources(options.qt5_srcbase, modules, source_zip)

    if sources_only:
        return

    ensure_sdp_sourced()

    if sys.platform.startswith('win32'):
        home = os.environ['HOMEPATH']
    else:
        home = os.environ['HOME']

    # Write info into REVISIONS file
    options.baseConfigureFlags = options.baseConfigureFlags + ' -opengl es2 -egl';

    # Go build Qt5
    try:
        build_qt5.build_qt5(options.qt5_installbase, options)
        print("Done building Qt5, installed in %s." % options.qt5_installbase)
    except:
        raise

def Options():
    o = build_qt5.Options()
    o.baseConfigureFlags = '-v -no-cups -no-icu -no-xcb -no-eglfs -no-javascript-jit -no-gtkstyle -no-iconv -opensource -no-pch -confirm-license -nomake demos -nomake examples -nomake docs'
    o.qt5_modules = 'qtbase qtxmlpatterns qtjsbackend qtdeclarative qtsvg qtgraphicaleffects'
    o.qt5_installbase = '/base/qt5'
    o.buildmode = 'release'
    o.qt5_version = '5.0.2'
    o.opengl = True
    return o

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='build Qt5 for QNX')
    parser.add_argument('--export-sources', dest='source_zip', help='ZIP file path to export Qt sources to')
    parser.add_argument('--sources-only', dest='sources_only', action='store_true', help='package sources only')
    parser.add_argument('--platform', dest='platform', default='qnx-armv7le-qcc', help='mkspec to use for building Qt')
    parser.add_argument('--host-system', dest='host_system', help='The platform to build the host tools for. By default the current system.')

    args = parser.parse_args()

    o = Options()
    o.xplatform = args.platform
    o.platform = args.host_system

    qnx_build_qt5(args.source_zip, args.sources_only, o)
