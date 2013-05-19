/* Source file created: 2013-05-18

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

#include "logsvccpp/client/DefaultConnection.h"
#include "logsvc_daemon/test/DummySocket.h"
#include "log/Deliverable.h"
#include "log/Receivable.h"
#include <egen/make_unique.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(testDefaultConnection)

struct F : mock::DummySocketKilledListener
{
  F() : live_sockets(0), dummy_socket(nullptr) {}
  ~F() {}

  std::unique_ptr<mock::DummySocket> create_dummy_socket()
  {
    std::unique_ptr<mock::DummySocket> socket(new mock::DummySocket(this));
    ++live_sockets;
    dummy_socket = socket.get();
    return std::move(socket);
  }

  virtual void socket_killed(const mock::DummySocket* socket_to_die)
  {
    BOOST_CHECK_EQUAL(dummy_socket, socket_to_die);
    --live_sockets;
  }

  int live_sockets;
  mock::DummySocket* dummy_socket;
};

BOOST_FIXTURE_TEST_CASE(manages_socket_lifetime, F)
{
  {
    logsvc::client::DefaultConnection connection(create_dummy_socket());
    BOOST_CHECK_EQUAL(1, live_sockets);
  }
  BOOST_CHECK_EQUAL(0, live_sockets);
}

BOOST_FIXTURE_TEST_CASE(no_socket_io_during_construction, F)
{
  logsvc::client::DefaultConnection connection(create_dummy_socket());
  BOOST_CHECK_EQUAL(0, dummy_socket->async_read_call_count);
  BOOST_CHECK_EQUAL(0, dummy_socket->written_bytes.size());
}

class DummyDeliverable : public logsvc::prot::Deliverable
{
public:
  virtual std::string get_header() const { return std::string("logsdumm\06\0\0\0", 12); }
  virtual std::string get_payload() const { return "foobar"; }
};

BOOST_FIXTURE_TEST_CASE(sending_deliverable_sends_data, F)
{
  logsvc::client::DefaultConnection connection(create_dummy_socket());
  DummyDeliverable dummy_deliverable;
  connection.send(dummy_deliverable);
  BOOST_CHECK_EQUAL(dummy_socket->written_bytes,
                    dummy_deliverable.get_header() + dummy_deliverable.get_payload());
}

BOOST_FIXTURE_TEST_CASE(sending_deliverable_waits_for_reply, F)
{
  logsvc::client::DefaultConnection connection(create_dummy_socket());
  DummyDeliverable dummy_deliverable;
  connection.send(dummy_deliverable);
  BOOST_CHECK_EQUAL(1, dummy_socket->async_read_call_count);
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
