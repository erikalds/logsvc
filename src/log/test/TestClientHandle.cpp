/* Source file created: 2013-04-01

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

#include "log/ClientHandle.h"
#include "log/Receivable.h"

using namespace logsvc::prot;

BOOST_AUTO_TEST_SUITE(testClientHandle)

BOOST_AUTO_TEST_CASE(can_create)
{
  ClientHandle ch(42);
}

BOOST_AUTO_TEST_CASE(can_compare)
{
  BOOST_CHECK(ClientHandle(42) == ClientHandle(42));
  BOOST_CHECK(!(ClientHandle(42) == ClientHandle(24)));

  BOOST_CHECK(ClientHandle(42) != ClientHandle(24));
  BOOST_CHECK(!(ClientHandle(42) != ClientHandle(42)));

  BOOST_CHECK(ClientHandle(24) < ClientHandle(42));
  BOOST_CHECK(!(ClientHandle(42) < ClientHandle(24)));
  BOOST_CHECK(!(ClientHandle(42) < ClientHandle(42)));

  BOOST_CHECK(ClientHandle(24) <= ClientHandle(42));
  BOOST_CHECK(ClientHandle(24) <= ClientHandle(24));
  BOOST_CHECK(!(ClientHandle(42) <= ClientHandle(24)));

  BOOST_CHECK(!(ClientHandle(24) > ClientHandle(42)));
  BOOST_CHECK(ClientHandle(42) > ClientHandle(24));
  BOOST_CHECK(!(ClientHandle(42) > ClientHandle(42)));

  BOOST_CHECK(!(ClientHandle(24) >= ClientHandle(42)));
  BOOST_CHECK(ClientHandle(42) >= ClientHandle(24));
  BOOST_CHECK(ClientHandle(42) >= ClientHandle(42));
}

BOOST_AUTO_TEST_CASE(get_header)
{
  ClientHandle ch(42);
  BOOST_CHECK_EQUAL(std::string("logsclnh\4\0\0\0", 12), ch.get_header());
}

BOOST_AUTO_TEST_CASE(get_payload)
{
  BOOST_CHECK_EQUAL("\x42\x43\x44\x45", ClientHandle(0x45444342).get_payload());
  BOOST_CHECK_EQUAL(std::string("\x42\0\0\0", 4), ClientHandle(0x42).get_payload());
}

BOOST_AUTO_TEST_CASE(is_a_Receivable)
{
  ClientHandle ch;
  BOOST_CHECK(dynamic_cast<Receivable*>(&ch) != nullptr);
}

BOOST_AUTO_TEST_CASE(read_payload)
{
  ClientHandle ch;
  ch.read_payload("\x42\x43\x44\x45");
  BOOST_CHECK(ClientHandle(0x45444342) == ch);
  ch.read_payload(std::string("\x42\0\0\0", 4));
  BOOST_CHECK(ClientHandle(0x42) == ch);
}

BOOST_AUTO_TEST_CASE(can_boost_check_equal)
{
  ClientHandle ch0, ch1;
  BOOST_CHECK_EQUAL(ch0, ch1);
}

BOOST_AUTO_TEST_CASE(can_stream_out)
{
  ClientHandle ch0(0x12345), ch1(0x654321);
  std::ostringstream ost0;
  ost0 << ch0;
  std::ostringstream ost1;
  ost1 << ch1;
  BOOST_CHECK_EQUAL(ost0.str(), "ClientHandle[0x12345]");
  BOOST_CHECK_EQUAL(ost1.str(), "ClientHandle[0x654321]");
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
