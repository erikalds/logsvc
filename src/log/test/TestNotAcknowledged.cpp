/* Source file created: 2013-03-27

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
#include "log/NotAcknowledged.h"

BOOST_AUTO_TEST_SUITE(testNotAcknowledged)

struct F
{
  F() : nack0("error"), nack1("fail") {}
  ~F() {}

  logsvc::prot::NotAcknowledged nack0, nack1;
};

BOOST_FIXTURE_TEST_CASE(can_get_reason, F)
{
  BOOST_CHECK_EQUAL("error", nack0.get_reason());
  BOOST_CHECK_EQUAL("fail", nack1.get_reason());
}

BOOST_FIXTURE_TEST_CASE(get_header, F)
{
  BOOST_CHECK_EQUAL(std::string("logsnack\x05\x00\x00\x00", 12),
                    nack0.get_header());
  BOOST_CHECK_EQUAL(std::string("logsnack\x04\x00\x00\x00", 12),
                    nack1.get_header());
}

BOOST_FIXTURE_TEST_CASE(get_payload, F)
{
  BOOST_CHECK_EQUAL("error", nack0.get_payload());
  BOOST_CHECK_EQUAL("fail", nack1.get_payload());
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
