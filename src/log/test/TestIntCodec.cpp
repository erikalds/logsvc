/* Source file created: 2013-04-06

  logsvc - logging as a service
  Copyright (C) 2013 Erik �ldstedt Sund

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
    Erik �ldstedt Sund
    Darres veg 14
    NO-7540 KL�BU
    NORWAY
*/

#include <boost/test/unit_test.hpp>
#include "log/int_codec.h"

using logsvc::prot::encode_32bit_int;
using logsvc::prot::decode_32bit_int;

BOOST_AUTO_TEST_SUITE(testIntCodec)

struct F
{
  F() {}
  ~F() {}
};

BOOST_FIXTURE_TEST_CASE(test_encode_32bit_int, F)
{
  BOOST_CHECK_EQUAL(std::string("\xba\xab\0\0", 4), encode_32bit_int(0xabba));
  BOOST_CHECK_EQUAL(std::string("\xc0\xff\xee\0", 4), encode_32bit_int(0xeeffc0));
  if (sizeof(std::size_t) > 4)
    BOOST_CHECK_THROW(encode_32bit_int(0x100000000), std::logic_error);
}

BOOST_FIXTURE_TEST_CASE(test_decode_32bit_int, F)
{
  BOOST_CHECK_EQUAL(0xc0ffee, decode_32bit_int(std::string("\xee\xff\xc0\0", 4)));
  BOOST_CHECK_EQUAL(0xabbadead, decode_32bit_int(std::string("\xad\xde\xba\xab", 4)));
  BOOST_CHECK_THROW(decode_32bit_int("123"), std::logic_error);
  BOOST_CHECK_THROW(decode_32bit_int("12345"), std::logic_error);
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
