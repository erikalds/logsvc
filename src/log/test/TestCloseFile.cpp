/* Source file created: 2013-04-07

  logsvc - logging as a service
  Copyright (C) 2013 Erik Åldstedt Sund

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  To contact the author, e-mail at erikalds@gmail.com or through
  regular mail:
    Erik Åldstedt Sund
    Darres veg 14
    NO-7540 KLÆBU
    NORWAY
*/

#include <boost/test/unit_test.hpp>
#include "log/CloseFile.h"
#include "log/FileHandle.h"
#include "log/test/DummyExecutor.h"

using namespace logsvc::prot;

BOOST_AUTO_TEST_SUITE(testCloseFile)

struct F
{
  F() : exec() {}
  ~F() {}

  std::unique_ptr<Deliverable> act(const FileHandle& fh)
  {
    CloseFile cf(fh);
    return cf.act(exec);
  }

  mock::DummyExecutor exec;
};

BOOST_FIXTURE_TEST_CASE(can_get_header, F)
{
  CloseFile cf(FileHandle(2));
  BOOST_CHECK_EQUAL(std::string("logsclos\4\0\0\0", 12), cf.get_header());
}

BOOST_FIXTURE_TEST_CASE(get_payload, F)
{
  CloseFile cf(FileHandle(2));
  BOOST_CHECK_EQUAL(std::string("\2\0\0\0", 4), cf.get_payload());
  cf = CloseFile(FileHandle(0x626d7544));
  BOOST_CHECK_EQUAL(std::string("\x44\x75\x6d\x62", 4), cf.get_payload());
}

BOOST_FIXTURE_TEST_CASE(is_a_Deliverable, F)
{
  CloseFile cf(FileHandle(2));
  BOOST_CHECK(dynamic_cast<Deliverable*>(&cf) != nullptr);
}

BOOST_FIXTURE_TEST_CASE(is_a_Receivable, F)
{
  CloseFile cf(FileHandle(2));
  BOOST_CHECK(dynamic_cast<Receivable*>(&cf) != nullptr);
}

BOOST_FIXTURE_TEST_CASE(act_calls_close_file, F)
{
  act(FileHandle(2));
  BOOST_CHECK(FileHandle(2) == exec.closed_file);

  act(FileHandle(3));
  BOOST_CHECK(FileHandle(3) == exec.closed_file);
}

BOOST_FIXTURE_TEST_CASE(act_positive, F)
{
  std::unique_ptr<Deliverable> deliverable = act(FileHandle(2));
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsackn\0\0\0\0", 12), deliverable->get_header());
}

BOOST_FIXTURE_TEST_CASE(act_negative, F)
{
  exec.close_file_fails = true;
  std::unique_ptr<Deliverable> deliverable = act(FileHandle(2));
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsnack\5\0\0\0", 12), deliverable->get_header());
  BOOST_CHECK_EQUAL("error", deliverable->get_payload());

  exec.error_string = "fail";
  deliverable = act(FileHandle(2));
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsnack\4\0\0\0", 12), deliverable->get_header());
  BOOST_CHECK_EQUAL("fail", deliverable->get_payload());
}

BOOST_FIXTURE_TEST_CASE(read_payload, F)
{
  CloseFile cf;
  FileHandle fh(2);
  BOOST_CHECK_EQUAL(fh.get_payload_length(), cf.get_payload_length());
  cf.read_payload(fh.get_payload());
  cf.act(exec);
  BOOST_CHECK(fh == exec.closed_file);

  fh = FileHandle(3);
  cf.read_payload(fh.get_payload());
  cf.act(exec);
  BOOST_CHECK(fh == exec.closed_file);
}

BOOST_AUTO_TEST_SUITE_END()

/*

  The defined macros are:

  BOOST_ERROR(message)
  BOOST_FAIL(message)
  BOOST_IS_DEFINED(symbol)

  BOOST_level(condition)
  BOOST_level_MESSAGE(condition, message)
  BOOST_level_THROW(expression, exception_type)
  BOOST_level_EXCEPTION(expression, exception_type, P)
  BOOST_level_NO_THROW(expression)
  BOOST_level_EQUAL(left_expression, right_expression)
  BOOST_level_NE(left_expression, right_expression)
  BOOST_level_LT(left_expression, right_expression)
  BOOST_level_LE(left_expression, right_expression)
  BOOST_level_GT(left_expression, right_expression)
  BOOST_level_GE(left_expression, right_expression)
  BOOST_level_CLOSE(left_expression, right_expression, percentage)
  BOOST_level_CLOSE_FRACTION(left_expression, right_expression, fraction)
  BOOST_level_SMALL(left_expression, right_expression, T)
  BOOST_level_PREDICATE(predicate_function, arguments)
  BOOST_level_EQUAL_COLLECTIONS(left_begin, left_end, right_begin, right_end)
  BOOST_level_BITWISE_EQUAL(left_expression, right_expression)

  Three levels of errors are defined:

  Level   | Errors counter | Test execution
  --------+----------------+---------------
  WARN    | not affected   | continues
  CHECK   | increased      | continues
  REQUIRE | increased      | aborts

 */
