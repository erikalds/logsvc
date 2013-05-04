/* Source file created: 2013-05-04

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

#define BOOST_TEST_MODULE "logsvccpp test module"
#include <boost/test/unit_test.hpp>

#include "logsvccpp/client/Host.h"
#include "logsvccpp/client/ConnectionFactory.h"
#include "logsvccpp/client/SessionConnection.h"

BOOST_AUTO_TEST_SUITE(testClientHost)

class DSCKilledListener
{
public:
  virtual ~DSCKilledListener() {}

  virtual void connection_killed() const = 0;
};

class DummySessionConnection : public logsvc::client::SessionConnection
{
public:
  DummySessionConnection(const DSCKilledListener* listener) : listener(listener) {}
  ~DummySessionConnection() { listener->connection_killed(); }

private:
  const DSCKilledListener* listener;
};

class DummyConnectionFactory : public logsvc::client::ConnectionFactory,
                               public DSCKilledListener
{
public:
  DummyConnectionFactory() : create_count(0), live_count(0) {}

  virtual std::unique_ptr<logsvc::client::SessionConnection> create_session() const
  {
    ++create_count;
    ++live_count;
    return std::unique_ptr<logsvc::client::SessionConnection>(new DummySessionConnection(this));
  }

  virtual void connection_killed() const
  {
    --live_count;
  }

  mutable int create_count;
  mutable int live_count;
};

struct F
{
  F() {}
  ~F() {}
};

BOOST_FIXTURE_TEST_CASE(constructor_creates_session_connection, F)
{
  DummyConnectionFactory connection_factory;
  logsvc::client::Host host("appname", connection_factory);
  BOOST_CHECK_EQUAL(1, connection_factory.create_count);
}

BOOST_FIXTURE_TEST_CASE(keeps_session_connection, F)
{
  DummyConnectionFactory connection_factory;
  logsvc::client::Host host("appname", connection_factory);
  BOOST_CHECK_EQUAL(1, connection_factory.live_count);
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
