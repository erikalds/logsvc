/* Source file created: 2013-03-03

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

#include "log/Deliverable.h"
#include "log/File.h"
#include "log/FileHandle.h"
#include "log/Receivable.h"
#include "log/test/DummyExecutor.h"

#define BOOST_TEST_MODULE "log test suite"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(testFile)

BOOST_AUTO_TEST_CASE(get_name_test)
{
  logsvc::prot::File f1(boost::filesystem::path("asdf.txt"));
  BOOST_CHECK_EQUAL(boost::filesystem::path("asdf.txt"), f1.get_name());

  logsvc::prot::File f2(boost::filesystem::path("../path/asdf.txt"));
  BOOST_CHECK_EQUAL(boost::filesystem::path("../path/asdf.txt"), f2.get_name());
}

BOOST_AUTO_TEST_CASE(read_payload)
{
  logsvc::prot::File f;
  BOOST_CHECK_THROW(f.get_name(), std::logic_error);

  f.read_payload("asdf.txt");
  BOOST_CHECK_EQUAL(boost::filesystem::path("asdf.txt"), f.get_name());

  f.read_payload("../path/asdf.txt");
  BOOST_CHECK_EQUAL(boost::filesystem::path("../path/asdf.txt"), f.get_name());
}

BOOST_AUTO_TEST_CASE(act_positive)
{
  logsvc::prot::File f(boost::filesystem::path("asdf.txt"));
  mock::DummyExecutor exec;
  std::unique_ptr<logsvc::prot::Deliverable> first_deliverable = f.act(exec);
  BOOST_CHECK_EQUAL(boost::filesystem::path("asdf.txt"), exec.opened_file);

  f.read_payload("../path/asdf.txt");
  std::unique_ptr<logsvc::prot::Deliverable> second_deliverable = f.act(exec);
  BOOST_CHECK_EQUAL(boost::filesystem::path("../path/asdf.txt"), exec.opened_file);

  BOOST_CHECK(first_deliverable != second_deliverable);
  BOOST_REQUIRE(first_deliverable != nullptr);

  BOOST_CHECK_EQUAL(std::string("logsfilh\x04\x00\x00\x00", 12),
                    first_deliverable->get_header());
  BOOST_CHECK_EQUAL(std::string("\x01\x00\x00\x00", 4),
                    first_deliverable->get_payload());

  BOOST_REQUIRE(second_deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsfilh\x04\x00\x00\x00", 12),
                    second_deliverable->get_header());
  BOOST_CHECK_EQUAL(std::string("\x02\x00\x00\x00", 4),
                    second_deliverable->get_payload());
}

BOOST_AUTO_TEST_CASE(act_negative)
{
  logsvc::prot::File f(boost::filesystem::path("asdf.txt"));
  mock::DummyExecutor exec;
  exec.open_file_fails = true;
  std::unique_ptr<logsvc::prot::Deliverable> deliverable = f.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsnack\x05\x00\x00\x00", 12),
                    deliverable->get_header());
  BOOST_CHECK_EQUAL(std::string("error", 5),
                    deliverable->get_payload());

  exec.error_string = "failed to open file";
  deliverable = f.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsnack\x13\x00\x00\x00", 12),
                    deliverable->get_header());
  BOOST_CHECK_EQUAL(std::string("failed to open file"),
                    deliverable->get_payload());
}

BOOST_AUTO_TEST_CASE(is_a_Receivable)
{
  logsvc::prot::File f;
  BOOST_CHECK(dynamic_cast<logsvc::prot::Receivable*>(&f) != nullptr);
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
