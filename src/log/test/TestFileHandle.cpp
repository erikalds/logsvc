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

#include "log/FileHandle.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(testFileHandle)

struct F
{
  F() {}
  ~F() {}
};

BOOST_FIXTURE_TEST_CASE(can_create, F)
{
  logsvc::prot::FileHandle fh(345);
}

BOOST_FIXTURE_TEST_CASE(FileHandle_eq_op, F)
{
  BOOST_CHECK(logsvc::prot::FileHandle(234) == logsvc::prot::FileHandle(234));
  BOOST_CHECK(!(logsvc::prot::FileHandle(42) == logsvc::prot::FileHandle(234)));
}

BOOST_FIXTURE_TEST_CASE(FileHandle_ineq_op, F)
{
  BOOST_CHECK(!(logsvc::prot::FileHandle(234) != logsvc::prot::FileHandle(234)));
  BOOST_CHECK(logsvc::prot::FileHandle(42) != logsvc::prot::FileHandle(234));
}

BOOST_FIXTURE_TEST_CASE(FileHandle_lt_op, F)
{
  BOOST_CHECK(logsvc::prot::FileHandle(123) < logsvc::prot::FileHandle(124));
  BOOST_CHECK(!(logsvc::prot::FileHandle(125) < logsvc::prot::FileHandle(124)));
  BOOST_CHECK(!(logsvc::prot::FileHandle(125) < logsvc::prot::FileHandle(125)));
}

BOOST_FIXTURE_TEST_CASE(FileHandle_le_op, F)
{
  BOOST_CHECK(logsvc::prot::FileHandle(223) <= logsvc::prot::FileHandle(224));
  BOOST_CHECK(!(logsvc::prot::FileHandle(225) <= logsvc::prot::FileHandle(224)));
  BOOST_CHECK(logsvc::prot::FileHandle(225) <= logsvc::prot::FileHandle(225));
}

BOOST_FIXTURE_TEST_CASE(FileHandle_gt_op, F)
{
  BOOST_CHECK(!(logsvc::prot::FileHandle(323) > logsvc::prot::FileHandle(324)));
  BOOST_CHECK(logsvc::prot::FileHandle(325) > logsvc::prot::FileHandle(324));
  BOOST_CHECK(!(logsvc::prot::FileHandle(325) > logsvc::prot::FileHandle(325)));
}

BOOST_FIXTURE_TEST_CASE(FileHandle_ge_op, F)
{
  BOOST_CHECK(!(logsvc::prot::FileHandle(423) >= logsvc::prot::FileHandle(424)));
  BOOST_CHECK(logsvc::prot::FileHandle(425) >= logsvc::prot::FileHandle(424));
  BOOST_CHECK(logsvc::prot::FileHandle(425) >= logsvc::prot::FileHandle(425));
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
