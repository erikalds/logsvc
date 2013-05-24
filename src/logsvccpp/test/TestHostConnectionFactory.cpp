/* Source file created: 2013-05-24

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
#include "logsvccpp/client/HostConnectionFactory.h"
#include "log/test/DummyExecutor.h"
#include "log/Client.h"

BOOST_AUTO_TEST_SUITE(testHostConnectionFactory)

struct F
{
  F() {}
  ~F() {}
};

// will not test create_session, since it actually does the networking with the
// asio socket

BOOST_FIXTURE_TEST_CASE(create_client_info_sets_appname, F)
{
  logsvc::client::HostConnectionFactory factory("asdf", "192.168.1.102");
  logsvc::prot::Client client = factory.create_client_info("appname");
  mock::DummyExecutor exec;
  client.act(exec);
  BOOST_CHECK_EQUAL(exec.client_name, "appname");
  client = factory.create_client_info("foo");
  client.act(exec);
  BOOST_CHECK_EQUAL(exec.client_name, "foo");
}

BOOST_FIXTURE_TEST_CASE(gives_correct_ip, F)
{
  logsvc::client::HostConnectionFactory factory("asdf", "192.168.1.104");
  logsvc::prot::Client client = factory.create_client_info("appname");
  mock::DummyExecutor exec;
  client.act(exec);
  BOOST_CHECK_EQUAL(exec.client_address, "192.168.1.104");

  factory = logsvc::client::HostConnectionFactory("asdf", "127.0.0.1");
  client = factory.create_client_info("appname");
  client.act(exec);
  BOOST_CHECK_EQUAL(exec.client_address, "127.0.0.1");
}

BOOST_FIXTURE_TEST_CASE(handles_ipv6_addresses, F)
{
  logsvc::client::HostConnectionFactory factory("asdf", "1010:2020:3030:4040:5050:6060:7070:8080");
  logsvc::prot::Client client = factory.create_client_info("appname");
  mock::DummyExecutor exec;
  client.act(exec);
  BOOST_CHECK_EQUAL(exec.client_address, "1010:2020:3030:4040:5050:6060:7070:8080");
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
