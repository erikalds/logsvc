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

from framework import SystemTests, TextProgressPublisher, TextResultsPublisher, TestLoader, StdoutAndLogPublisher

logsvcd = None

class Command:
    def __init__(self, command):
        cmd_path = command[0] if type(command) == list else command
        if not os.path.exists(cmd_path):
            raise Exception("File not found: %s" % cmd_path)

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

def compile_loglinetofile():
    return make("loglinetofile")

def compile_logviastream():
    return make("logviastream")

def compile_cloglinetofile():
    return make("cloglinetofile")

def start_logsvcd():
    global logsvcd

    proc = subprocess.Popen("build/logsvcd", stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    if proc.poll() == None:
        logsvcd = proc
        return (True, "")
    else:
        return (False, proc.communicate()[1])

def check_logged_lines(fname, app, lines):
    od = dict(app=app)
    if not os.path.exists(fname):
        return (False, "   - %(app)s did not even create the file.\n" % od)

    contents = None
    with open(fname, 'r') as fd:
        contents = fd.read()

    mo_log_opened = re.search("Log opened by \"%(app)s\"" % od, contents)
    mo_log_closed = re.search("Log closed by \"%(app)s\"" % od, contents)
    mo_lines = [re.search("] %s\n" % x, contents) for x in lines]

    report = ""
    if not mo_log_opened:
        report += "   - Missing \"Log opened by ...\" message.\n"
    if not mo_log_closed:
        report += "   - Missing \"Log closed by ...\" message.\n"
    for i in range(len(mo_lines)):
        if not mo_lines[i]:
            report += "   - Missing logged message line %d.\n" % i

    if mo_log_opened and mo_log_closed:
        if mo_log_opened.start() > mo_log_closed.start():
            report += "   - \"Log opened\" after \"Log closed\""

    if mo_lines:
        if mo_log_opened and mo_lines[0]:
            if mo_log_opened.start() > mo_lines[0].start():
                report += '   - "Log opened" after logged message line 1\n'
        if mo_log_closed and mo_lines[0]:
            if mo_log_closed.start() < mo_lines[0].start():
                report += '   - "Log closed" before logged message line 1\n'
        if len(mo_lines) > 1:
            for i in range(1, len(mo_lines)):
                if mo_lines[i - 1] and mo_lines[i]:
                    if mo_lines[i - 1].start() > mo_lines[i].start():
                        report += '   - Line %d before line %d' % (i, i - 1)

    if report:
        report += "\nLog contents was:\n" + contents + "\n"

    return (not bool(report), report)

def log_line_to_file_test():
    fname = "system_test_output_log.txt"
    if os.path.exists(fname):
        os.unlink(fname)

    result = run(["systest/loglinetofile", fname, "a", "few", "strings"], 10)
    if not result[0]:
        return result

    return check_logged_lines(fname, "loglinetofile",
                              ["a", "few", "strings"])

def log_string_via_stream_test():
    fname = "system_test_output_log.txt"
    if os.path.exists(fname):
        os.unlink(fname)

    result = run(["systest/logviastream", fname, "a string"], 10)
    if not result[0]:
        return result

    return check_logged_lines(fname, "logviastream", ["a string"])

def c_log_line_to_file_test():
    fname = "system_test_output_log.txt"
    if os.path.exists(fname):
        os.unlink(fname)

    result = run(["systest/cloglinetofile", fname, "a", "few", "strings"],
                 10)
    if not result[0]:
        return result

    return check_logged_lines(fname, "cloglinetofile",
                              ['a', 'few', 'strings'])

def py_log_line_to_file_test():
    fname = "system_test_output_log.txt"
    if os.path.exists(fname):
        os.unlink(fname)

    sys.path.append('./build')
    try:
        from logsvcpy import Log, Host
        host = Host("pyapp", "localhost")
        log = Log(fname, host)

        log.logln('a')
        log.logln('few')
        log.logln('strings')

        return check_logged_lines(fname, 'pyapp', ['a', 'few', 'strings'])

    finally:
        sys.path.remove(sys.path[-1])

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
            "010_compile_loglinetofile": compile_loglinetofile,
            "010_compile_logviastream": compile_logviastream,
            "011_compile_cloglinetofile": compile_cloglinetofile,
            "015_start_logsvcd": start_logsvcd,
            "020_run_Host_test": run_Host_test,
            "020_run_Log_test": run_Log_test,
            "020_run_OutStream_test": run_OutStream_test,
            "199_log_line_to_file_test": log_line_to_file_test,
            "200_log_string_via_stream_test": log_string_via_stream_test,
            "300_c_log_line_to_file_test": c_log_line_to_file_test,
            "300_py_log_line_to_file_test": py_log_line_to_file_test,
            "999_HUP_logsvcd" : hup_logsvcd
            }
        return tests


def main(argv):
    os.environ['LD_LIBRARY_PATH'] = "build"

    tests = SystemTests(MyTestLoader())
    progress_publisher = TextProgressPublisher()
    results_publisher = TextResultsPublisher()
    if len(argv) > 1:
        progress_publisher = StdoutAndLogPublisher(argv[1])
        results_publisher = progress_publisher

    tests.run(progress_publisher)
    tests.publish_results(results_publisher)

    return tests.regression_count()

if __name__ == '__main__':
    sys.exit(main(sys.argv))
