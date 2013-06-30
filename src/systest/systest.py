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
import threading
import time

from framework import SystemTests, TextProgressPublisher, TextResultsPublisher, TestLoader

logsvcd = None

class Command:
    def __init__(self, command):
        self._command = command

    def run(self, timeout):
        def in_thread():
            self.exitcode = None
            self.proc = subprocess.Popen(self._command,
                                         stdout=subprocess.PIPE,
                                         stderr=subprocess.PIPE)
            self.exitcode = self.proc.wait()

        thread = threading.Thread(target=in_thread)
        thread.start()
        if timeout != None:
            thread.join(timeout)
        else:
            thread.join()

        if self.exitcode != None:
            stdout, stderr = self.proc.communicate()
            return (self.exitcode == 0, stderr)
        else:
            self.proc.terminate()
            joined_cmd = " ".join(self._command) \
                if type(self._command) == type([]) \
                else self._command
            return (False, "Subprocess timed out (%s)\n%s" % (joined_cmd,
                                                              self.proc.stderr.read()))



def run(command, timeout=None):
    cmd = Command(command)
    return cmd.run(timeout)

def make(target):
    return run(["/usr/bin/make", "-C", "systest", target])

def compile_Host_test():
    return make("Host")

def run_Host_test():
    return run("systest/Host", 2)

def compile_Log_test():
    return make("Log")

def run_Log_test():
    return run("systest/Log", 2)

def compile_OutStream_test():
    return make("OutStream")

def run_OutStream_test():
    return run("systest/OutStream", 2)

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

def log_string_to_file_test():
    fname = "system_test_output_log.txt"
    if os.path.exists(fname):
        os.unlink(fname)

    result = run(["systest/logtofile", fname, "a string"], 10)
    if not result[0]:
        return result

    if not os.path.exists(fname):
        return (False, "   - logtofile did not even create the file.\n")

    contents = None
    with open(fname, 'r') as fd:
        contents = fd.read()

    mo_log_opened = re.search("Log opened by \"logtofile\"", contents)
    mo_log_closed = re.search("Log closed by \"logtofile\"", contents)
    mo_string = re.search("a string", contents)

    report = ""
    if not mo_log_opened:
        report += "   - Missing \"Log opened by ...\" message.\n"
    if not mo_log_closed:
        report += "   - Missing \"Log closed by ...\" message.\n"
    if not mo_string:
        report += "   - Missing logged message.\n"
    if mo_log_opened and mo_string and mo_log_opened.start() > mo_string.start():
        report += "   - Logged message before \"Log opened\" message.\n"
    if mo_log_closed and mo_string and mo_log_closed.start() < mo_string.start():
        report += "   - Logged message after \"Log closed\" message.\n"

    if report:
        report += "\nLog contents was:\n" + contents + "\n"

    return (not bool(report), report)

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
        tests = {
            "010_compile_Host_test": compile_Host_test,
            "010_compile_Log_test": compile_Log_test,
            "010_compile_OutStream_test": compile_OutStream_test,
            "010_compile_logtofile": compile_logtofile,
            "015_start_logsvcd": start_logsvcd,
            "020_run_Host_test": run_Host_test,
            "020_run_Log_test": run_Log_test,
            "020_run_OutStream_test": run_OutStream_test,
            "200_log_string_to_file_test": log_string_to_file_test,
            "999_HUP_logsvcd" : hup_logsvcd
            }
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
