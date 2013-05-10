# -*- coding: utf-8 -*-

# Source file created: 2013-05-10
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
import sys

class TestLoader:
    def load_tests(self):
        raise TypeError("A TestLoader subclass must implement the load_tests "
                        "member function.")


class SystemTests:
    def __init__(self, test_loader):
        assert(issubclass(test_loader.__class__, TestLoader))
        self._test_loader = test_loader
        self._tests = self._load_tests()
        self._earlier_passed_tests = self._load_passed_tests()
        self._regressions = []
        self._passed_tests = []
        self._failed_tests = []

    def run(self, progress_publisher):
        progress_publisher.tests_are_about_to_start(len(self._tests))
        self._regressions = []
        self._passed_tests = []
        self._failed_tests = []

        sorted_tests = self._tests.keys()
        sorted_tests.sort()
        for test in sorted_tests:
            progress_publisher.test_is_about_to_start(test)

            testresult = self._run_test(test)

            if not testresult[0]:
                if test in self._earlier_passed_tests:
                    self._regressions.append(test)

                self._failed_tests.append(test)
                progress_publisher.test_failed(test, testresult[1],
                                               test in self._earlier_passed_tests)
            else:
                progress_publisher.test_passed(test)
                self._passed_tests.append(test)

        self._write_passed_tests()

    def publish_results(self, results_publisher):
        results_publisher.report_passed_tests(self._passed_tests)
        results_publisher.report_failed_tests(self._failed_tests)
        results_publisher.report_regressions(self._regressions)
        results_publisher.report_status(not self._regressions)

    def regression_count(self):
        return len(self._regressions)

    def _load_tests(self):
        return self._test_loader.load_tests()

    def _load_passed_tests(self):
        if not os.path.exists("passed_tests.txt"):
            return []

        with open("passed_tests.txt", 'r') as fd:
            return [line for line in fd.readlines() if line]

    def _write_passed_tests(self):
        with open("passed_tests.txt", 'w') as fd:
            fd.writelines(self._passed_tests)

    def _parse_type(self, obj):
        t = str(type(obj))
        mo = re.match("<type '([^']+)'>", t)
        if not mo:
            return t
        else:
            return mo.group(1)

    def _run_test(self, test):
        test_function = self._tests[test]
        try:
            return test_function()
        except Exception as e:
            return (False, "%s: %s\n" % (parse_type(e), e))
        except:
            return (False, "Unknown exception occurred")


class TextProgressPublisher:
    def tests_are_about_to_start(self, test_count):
        print("Running %d system tests..." % test_count)

    def test_is_about_to_start(self, test):
        sys.stdout.write(" * %s... " % test)
        sys.stdout.flush()

    def test_failed(self, test, message, is_regression):
        print("FAILED")
        if is_regression:
            print("########## REGRESSION ##########")

        print(message)

    def test_passed(self, test):
        print("OK")


class TextResultsPublisher:
    def report_passed_tests(self, passed_tests):
        print("Passed tests:    %d" % len(passed_tests))

    def report_failed_tests(self, failed_tests):
        print("Failed tests:    %d" % len(failed_tests))

    def report_regressions(self, regressions):
        print("Regressed tests: %d" % len(regressions))

    def report_status(self, success):
        print("\nsystemtest: *** %s ***\n" % ("OK" if success else "FAILED"))
