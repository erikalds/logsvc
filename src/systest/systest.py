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
import re
import subprocess
import sys
import time

from framework import SystemTests, TextProgressPublisher, TextResultsPublisher, TestLoader

logsvcd = None

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

def start_logsvcd():
    global logsvcd

    proc = subprocess.Popen("build/logsvcd", stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    if proc.poll() == None:
        logsvcd = proc
        return (True, "")
    else:
        return (False, proc.communicate()[1])

def hup_logsvcd():
    global logsvcd

    if logsvcd.poll() != None:
        return (False, "logsvcd process has died prematurely")
    logsvcd.send_signal(subprocess.signal.SIGHUP)
    then = time.time()
    while logsvcd.poll() == None:
        now = time.time()
        if (now - then) > 1: # second
            logsvcd.send_signal(subprocess.signal.SIGKILL)
            logsvcd.wait()
            return (False, "logsvcd process failed to respond to SIGHUP within one second")

    return (True, "testing")

class MyTestLoader(TestLoader):
    def load_tests(self):
        tests = { "000_compile_Host_test": compile_Host_test,
                  "000_compile_Log_test": compile_Log_test,
                  "000_compile_OutStream_test": compile_OutStream_test,
                  "000_compile_logtofile": compile_logtofile,
                  "100_start_logsvcd": start_logsvcd,
                  "999_HUP_logsvcd" : hup_logsvcd }
        return tests


def main(argv):
    os.environ['LD_LIBRARY_PATH'] = "build"

    tests = SystemTests(MyTestLoader())
    progress_publisher = TextProgressPublisher()
    tests.run(progress_publisher)
    results_publisher = TextResultsPublisher()
    tests.publish_results(results_publisher)

    return tests.regression_count()

if __name__ == '__main__':
    sys.exit(main(sys.argv))
