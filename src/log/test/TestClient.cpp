/* Source file created: 2013-03-09

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
#include "log/Client.h"
#include "log/Receivable.h"
#include <boost/asio/ip/address.hpp>

BOOST_AUTO_TEST_SUITE(testClient)

BOOST_AUTO_TEST_CASE(get_name)
{
  logsvc::prot::Client client("name");
  BOOST_CHECK_EQUAL("name", client.get_name());

  client = logsvc::prot::Client("another name");
  BOOST_CHECK_EQUAL("another name", client.get_name());
}

BOOST_AUTO_TEST_CASE(get_ip)
{
  boost::asio::ip::address_v4 loopback((127ul << 24) + 1);
  logsvc::prot::Client client("name", loopback);
  BOOST_CHECK_EQUAL(loopback, client.get_ip_address());

  boost::asio::ip::address_v4 another(((1 << 24) + (2 << 16) + (3 << 8) + 4ul));
  client = logsvc::prot::Client("name", another);
  BOOST_CHECK_EQUAL(another, client.get_ip_address());
}

BOOST_AUTO_TEST_CASE(sets_name_when_ip_is_set)
{
  boost::asio::ip::address_v4 loopback((127ul << 24) + 1);
  logsvc::prot::Client client("name", loopback);
  BOOST_CHECK_EQUAL("name", client.get_name());

  client = logsvc::prot::Client("another name", loopback);
  BOOST_CHECK_EQUAL("another name", client.get_name());
}

BOOST_AUTO_TEST_CASE(uses_loopback_address_when_ip_not_set)
{
  boost::asio::ip::address_v4 loopback((127 << 24) + 1ul);
  logsvc::prot::Client client("name");
  BOOST_CHECK_EQUAL(loopback, client.get_ip_address());
}

BOOST_AUTO_TEST_CASE(is_a_Receivable)
{
  logsvc::prot::Client client("name");
  BOOST_CHECK(dynamic_cast<logsvc::prot::Receivable*>(&client) != nullptr);
}

BOOST_AUTO_TEST_CASE(can_read_payload)
{
  logsvc::prot::Client client;
  client.read_payload("v4\xc0\xa8\1\x2dthename");
  BOOST_CHECK_EQUAL("thename", client.get_name());
  BOOST_CHECK_EQUAL(boost::asio::ip::address_v4((192ul << 24)
                                                + (168 << 16)
                                                + (1 << 8)
                                                + 45),
                    client.get_ip_address());

  client.read_payload("v6\x10\x10\x20\x20\x30\x30\x40\x40\x50\x50\x60\x60\x70\x70\x80\x80"
                      "aname");
  BOOST_CHECK_EQUAL("aname", client.get_name());
  std::array<unsigned char, 16> bytes = {{ 0x10, 0x10, 0x20, 0x20, 0x30, 0x30,
                                           0x40, 0x40, 0x50, 0x50, 0x60, 0x60,
                                           0x70, 0x70, 0x80, 0x80 }};
  BOOST_CHECK_EQUAL(boost::asio::ip::address_v6(bytes),
                    client.get_ip_address());
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
