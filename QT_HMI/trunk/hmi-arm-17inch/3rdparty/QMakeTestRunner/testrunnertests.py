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

import unittest, subprocess, sys, re

# The test mostly ensures that the test runner is able to handle all kinds of 
# input configurations. For example, it tests if the runner works if no library 
# path is specified, et cetera.
class TestRunnerTests( unittest.TestCase ):
    def _runTestRunner( self, args ):
        return subprocess.check_output( args )

    def testHelp( self ):
        args = [sys.executable, 'testrunner.py', '-h']
        try:
            self.assertTrue( self._runTestRunner( args ).find( '--verbose' ) != -1 )
        except subprocess.CalledProcessError as e:
            self.fail( str( e ) )

    def testNoPaths( self ):
        args = [sys.executable, 'testrunner.py', '--verbose', sys.executable, '-c', 'print("Test output")']
        try:
            output = self._runTestRunner( args )
            self.assertTrue( output.find( 'Test output' ) != -1 )
            self.assertTrue( re.search( 'Framework paths:\s+none', output ) )
            self.assertTrue( re.search( 'Library directories:\s+none', output ) )
        except subprocess.CalledProcessError as e:
            self.fail( str( e ) )

    def testLibPath( self ):
        args = [sys.executable, 'testrunner.py', '--verbose',
                '--lib-path', 'test_lib_path',
                sys.executable, '-c', 'print("Test output")']
        try:
            output = self._runTestRunner( args )
            self.assertTrue( output.find( 'Test output' ) != -1 )
            self.assertTrue( re.search( 'Framework paths:\s+none', output ) )
            self.assertTrue( re.search( 'Library directories:\s+test_lib_path', output ) )
        except subprocess.CalledProcessError as e:
            self.fail( str( e ) )

    def testFrameworkPath( self ):
        args = [sys.executable, 'testrunner.py', '--verbose',
                '--framework-path', 'test_framework_path',
                sys.executable, '-c', 'print("Test output")']
        try:
            output = self._runTestRunner( args )
            self.assertTrue( output.find( 'Test output' ) != -1 )
            self.assertTrue( re.search( 'Framework paths:\s+test_framework_path', output ) )
            self.assertTrue( re.search( 'Library directories:\s+none', output ) )
        except subprocess.CalledProcessError as e:
            self.fail( str( e ) )

    def testLibPathAndFrameworkPath( self ):
        args = [sys.executable, 'testrunner.py', '--verbose',
                '--lib-path', 'test_lib_path',
                '--framework-path', 'test_framework_path',
                sys.executable, '-c', 'print("Test output")']
        try:
            output = self._runTestRunner( args )
            self.assertTrue( output.find( 'Test output' ) != -1 )
            self.assertTrue( re.search( 'Framework paths:\s+test_framework_path', output ) )
            self.assertTrue( re.search( 'Library directories:\s+test_lib_path', output ) )
        except subprocess.CalledProcessError as e:
            self.fail( str( e ) )

if __name__ == "__main__":
    unittest.main()
