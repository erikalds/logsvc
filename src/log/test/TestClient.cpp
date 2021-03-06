/* Source file created: 2013-03-09

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
#include "log/test/DummyExecutor.h"
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

BOOST_AUTO_TEST_CASE(get_payload_length)
{
  logsvc::prot::Client client(42);
  BOOST_CHECK_EQUAL(42, client.get_payload_length());

  client = logsvc::prot::Client("a_name");
  BOOST_CHECK_EQUAL(2 + 4 + 6, client.get_payload_length());

  std::array<unsigned char, 16> bytes = {{ 0x10, 0x10, 0x20, 0x20, 0x30, 0x30,
                                           0x40, 0x40, 0x50, 0x50, 0x60, 0x60,
                                           0x70, 0x70, 0x80, 0x80 }};
  boost::asio::ip::address_v6 ipv6_address(bytes);
  client = logsvc::prot::Client("name", ipv6_address);
  BOOST_CHECK_EQUAL(2 + 16 + 4, client.get_payload_length());
}

BOOST_AUTO_TEST_CASE(can_read_payload)
{
  logsvc::prot::Client client(2 + 4 + 7);
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

BOOST_AUTO_TEST_CASE(act_sets_client_name_and_address)
{
  boost::asio::ip::address_v4 loopback((127 << 24) + 1ul);
  logsvc::prot::Client client("name", loopback);
  mock::DummyExecutor exec;
  client.act(exec);
  BOOST_CHECK_EQUAL(exec.client_name, "name");
  BOOST_CHECK_EQUAL(exec.client_address, "127.0.0.1");

  client = logsvc::prot::Client("another_name",
                                boost::asio::ip::address_v4((127 << 24) + 2ul));
  client.act(exec);
  BOOST_CHECK_EQUAL(exec.client_name, "another_name");
  BOOST_CHECK_EQUAL(exec.client_address, "127.0.0.2");
}

BOOST_AUTO_TEST_CASE(act_positive)
{
  logsvc::prot::Client client("name");
  mock::DummyExecutor exec;
  exec.client_handle = logsvc::prot::ClientHandle(0x42);
  std::unique_ptr<logsvc::prot::Deliverable> deliverable = client.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsclnh\4\0\0\0", 12), deliverable->get_header());
  BOOST_CHECK_EQUAL(std::string("\x42\0\0\0", 4), deliverable->get_payload());
}

BOOST_AUTO_TEST_CASE(act_negative)
{
  logsvc::prot::Client client("name");
  mock::DummyExecutor exec;
  exec.error_on_set_client_info = true;
  std::unique_ptr<logsvc::prot::Deliverable> deliverable = client.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsnack\5\0\0\0", 12), deliverable->get_header());
  BOOST_CHECK_EQUAL("error", deliverable->get_payload());

  exec.error_string = "fail";
  deliverable = client.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsnack\4\0\0\0", 12), deliverable->get_header());
  BOOST_CHECK_EQUAL("fail", deliverable->get_payload());
}

BOOST_AUTO_TEST_CASE(is_a_Deliverable)
{
  logsvc::prot::Client client("name");
  BOOST_CHECK(dynamic_cast<logsvc::prot::Deliverable*>(&client) != nullptr);
}

BOOST_AUTO_TEST_CASE(get_header)
{
  logsvc::prot::Client client_v4("name");
  BOOST_CHECK_EQUAL(std::string("logsclnt\x0a\0\0\0", 12), client_v4.get_header());

  std::array<unsigned char, 16> bytes = {{ 0x10, 0x10, 0x20, 0x20, 0x30, 0x30,
                                           0x40, 0x40, 0x50, 0x50, 0x60, 0x60,
                                           0x70, 0x70, 0x80, 0x80 }};
  boost::asio::ip::address_v6 addrv6(bytes);
  logsvc::prot::Client client_v6("the_name", addrv6);
  BOOST_CHECK_EQUAL(std::string("logsclnt\x1a\0\0\0", 12), client_v6.get_header());
}

BOOST_AUTO_TEST_CASE(get_payload)
{
  logsvc::prot::Client client_v4("name");
  BOOST_CHECK_EQUAL(std::string("v4\x7f\0\0\1name", 10), client_v4.get_payload());
  client_v4 = logsvc::prot::Client("the_name", boost::asio::ip::address_v4((42u << 24) + (24u << 16) + (33u << 8) + 10));
  BOOST_CHECK_EQUAL(std::string("v4\x2a\x18\x21\x0athe_name", 14), client_v4.get_payload());

  std::array<unsigned char, 16> bytes0 = {{ 0x10, 0x10, 0x20, 0x20, 0x30, 0x30,
                                            0x40, 0x40, 0x50, 0x50, 0x60, 0x60,
                                            0x70, 0x70, 0x80, 0x80 }};
  logsvc::prot::Client client_v6("name", boost::asio::ip::address_v6(bytes0));
  BOOST_CHECK_EQUAL(std::string("v6\x10\x10\x20\x20\x30\x30\x40\x40\x50\x50\x60\x60\x70\x70\x80\x80name", 22), client_v6.get_payload());

  std::array<unsigned char, 16> bytes1 = {{ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                                            0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
                                            0x1c, 0x1d, 0x1e, 0x1f }};
  client_v6 = logsvc::prot::Client("the_name", boost::asio::ip::address_v6(bytes1));
  BOOST_CHECK_EQUAL(std::string("v6\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1fthe_name", 26), client_v6.get_payload());

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
