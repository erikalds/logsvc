#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Source file created: 2013-05-03
#
# logsvc - logging as a service
# Copyright (C) 2013 Erik Åldstedt Sund
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# To contact the author, e-mail at erikalds@gmail.com or through
# regular mail:
#   Erik Åldstedt Sund
#   Darres veg 14
#   NO-7540 KLÆBU
#   NORWAY

import os
import subprocess
import sys

def make(target):
    proc = subprocess.Popen(["/usr/bin/make", "-C", "systest", target],
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    exitcode = proc.wait()
    return (exitcode == 0, proc.communicate()[1])

def compile_Host_test():
    return make("Host")

def compile_Log_test():
    return make("Log")

def compile_OutStream_test():
    return make("OutStream")

def compile_logtofile():
    return make("logtofile")

def can_start_logsvcd():
    proc = subprocess.Popen("build/logsvcd", stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    if proc.poll() == None:
        proc.send_signal(subprocess.signal.SIGKILL)
        return (True, "")
    else:
        return (False, proc.communicate()[1])


def load_tests():
    tests = { "compile_Host_test": compile_Host_test,
              "compile_Log_test": compile_Log_test,
              "compile_OutStream_test": compile_OutStream_test,
              "compile_logtofile": compile_logtofile,
              "can_start_logsvcd": can_start_logsvcd }
    return tests

def load_passed_tests():
    if not os.path.exists("passed_tests.txt"):
        return []

    with open("passed_tests.txt", 'r') as fd:
        return [line for line in fd.readlines() if line]

def main(argv):
    os.environ['LD_LIBRARY_PATH'] = "build"

    print("Running system test suite...")
    tests = load_tests()
    print("Running %d tests..." % len(tests))
    earlier_passed_tests = load_passed_tests()
    regressions = []
    passed_tests = []
    failed_tests = []
    for test in tests:
        sys.stdout.write(" * %s... " % test)
        sys.stdout.flush()
        testresult = tests[test]()
        if not testresult[0]:
            print("FAILED")
            if test in earlier_passed_tests:
                print("########## REGRESSION ##########")
                regressions.append(test)

            failed_tests.append(test)
            print(testresult[1])
        else:
            print("OK")
            passed_tests.append(test)
            if test not in earlier_passed_tests:
                earlier_passed_tests.append(test)

    with open("passed_tests.txt", 'w') as fd:
        fd.writelines(earlier_passed_tests)

    print("Passed tests:    %d" % len(passed_tests))
    print("Failed tests:    %d" % len(failed_tests))
    print("Regressed tests: %d" % len(regressions))

    if not regressions:
        print("\nsystemtest: *** OK ***\n")
    else:
        print("\nsystemtest: *** FAILED ***\n")

    return len(regressions)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
