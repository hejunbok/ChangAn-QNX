#!/usr/bin/env python
# 
# This file is part of QMakeTestRunner - Python Test Runner for Qt and QMake.
#
# (C) Agile Workers Software, 2012.
# Author: Mirko Boehm <mirko@agile-workers.com>
# 
# QMakeTestRunner file is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published by 
# the Free Software Foundation, version 3 of the License.
# 
# This file is distributed in the hope that it will be useful,  but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with 
# QMakeTestRunner. If not, see http://www.gnu.org/licenses/.

import argparse
import os
import sys
import subprocess
import platform

# First, parse the command line arguments, and generate help (-h) in the process:
parser = argparse.ArgumentParser( prog = 'QMakeTestRunner',
                                  description = '''\
Python Test Runner for Qt and QMake, (C) Agile Workers Software, 2012.
Licensed under the GPL v3.
''' )
parser.add_argument( '-l', '--lib-path', dest = 'libPaths', required = False, action = 'append', default = [],
                     help = 'Library path to be added to the executable run time environment' )
# parser.add_argument( '-t', '--test-excutable', dest = 'testExecutable', required = False, action = 'store',
#                      help = 'A test executable to be executed' )
parser.add_argument( '-v', '--verbose', dest = 'verbose', required = False, action = 'store_true',
                     help = 'Toggle extra debug output' )
parser.add_argument( '-f', '--framework-path', dest = 'frameworkPaths', required = False, action = 'append', default = [],
                     help = 'Mac framework paths to be added to the executable run time environment' )
parser.add_argument( 'testExecutable', metavar = 'test', nargs = '?', type = str,
                     help = 'Test executable to be started' )
arguments, test_arguments = parser.parse_known_args()

# A function to print information if verbose is set:
def v_print( txt ):
    if arguments.verbose:
        print( txt )

# Some diagnostics:
v_print( "Library directories:   {0}".format( ', '.join( arguments.libPaths ) or 'none' ) )
v_print( "Framework paths:       {0}".format( ', '.join( arguments.frameworkPaths ) or 'none' ) )
v_print( "Test executable:       {0}".format( arguments.testExecutable or 'not specified' ) )
v_print( "Verbose:               {0}".format( 'yes' if arguments.verbose else 'no' ) )
v_print( "Test arguments:        {0}".format( ' '.join( test_arguments ) or 'none' ) )

# Determine which environment variable to change:
libPathVariable = ''
if 'Windows' in platform.platform():
    libPathVariable = 'PATH'
elif 'Darwin' in platform.platform():
    libPathVariable = 'DYLD_LIBRARY_PATH'
elif 'Linux' in platform.platform():
    libPathVariable = 'LD_LIBRARY_PATH'
else:
    print( 'Unsupported platform "{0}", it should be easy enough to add :-)!'.format( platform.platform() ) )
    sys.exit( 1 )

# Assemble library path and set it in the environment:
original_ld_library_path = '' if not os.environ.has_key( libPathVariable ) else os.environ[libPathVariable]
ld_library_path_elements = filter( len, [] + arguments.libPaths + original_ld_library_path.split( os.pathsep ) )
new_ld_library_path = os.pathsep.join( ld_library_path_elements )
v_print( 'Library search path:   {0}'.format( new_ld_library_path ) )
if new_ld_library_path: os.environ[libPathVariable] = new_ld_library_path

# Assemble framework path and set it in the environment:
frameworkPathVariable = 'DYLD_FRAMEWORK_PATH'
original_framework_path = '' if not os.environ.has_key( frameworkPathVariable ) else os.environ[frameworkPathVariable]
framework_path_elements = filter( len, [] + arguments.frameworkPaths + original_framework_path.split( os.pathsep ) )
new_framework_path = os.pathsep.join( framework_path_elements )
v_print( 'Framework search path: {0}'.format( new_framework_path ) )
if new_framework_path: os.environ[frameworkPathVariable] = new_framework_path

# Run the test program and return with it's exit code:
sys.exit( subprocess.call( [ arguments.testExecutable ] + test_arguments ) )
