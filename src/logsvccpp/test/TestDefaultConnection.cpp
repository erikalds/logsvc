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
#include "logsvccpp/SocketError.h"
#include "logsvc_daemon/test/DummyReceivableFactory.h"
#include "logsvc_daemon/test/DummySocket.h"
#include "log/test/DummyExecutor.h"
#include "log/Deliverable.h"
#include "log/Receivable.h"
#include <egen/lookup.h>
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

  std::unique_ptr<mock::DummyReceivableFactory> create_dummy_receivable_factory()
  {
    std::unique_ptr<mock::DummyReceivableFactory> dummy(new mock::DummyReceivableFactory);
    drf = dummy.get();
    return std::move(dummy);
  }

  logsvc::client::DefaultConnection create_connection()
  {
    return logsvc::client::DefaultConnection(create_dummy_socket(),
                                             create_dummy_receivable_factory());
  }

  virtual void socket_killed(const mock::DummySocket* socket_to_die)
  {
    BOOST_CHECK_EQUAL(dummy_socket, socket_to_die);
    --live_sockets;
  }

  int live_sockets;
  mock::DummySocket* dummy_socket;
  mock::DummyReceivableFactory* drf;
};

BOOST_FIXTURE_TEST_CASE(manages_socket_lifetime, F)
{
  {
    logsvc::client::DefaultConnection connection = create_connection();
    BOOST_CHECK_EQUAL(1, live_sockets);
  }
  BOOST_CHECK_EQUAL(0, live_sockets);
}

BOOST_FIXTURE_TEST_CASE(no_socket_io_during_construction, F)
{
  logsvc::client::DefaultConnection connection = create_connection();
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
  logsvc::client::DefaultConnection connection = create_connection();
  DummyDeliverable dummy_deliverable;
  connection.send(dummy_deliverable);
  BOOST_CHECK_EQUAL(dummy_socket->written_bytes,
                    dummy_deliverable.get_header() + dummy_deliverable.get_payload());
}

BOOST_FIXTURE_TEST_CASE(sending_deliverable_waits_for_reply, F)
{
  logsvc::client::DefaultConnection connection = create_connection();
  DummyDeliverable dummy_deliverable;
  connection.send(dummy_deliverable);
  BOOST_CHECK_EQUAL(1, dummy_socket->async_read_call_count);
}

BOOST_FIXTURE_TEST_CASE(sending_deliverable_tries_to_read_bytes_for_a_header, F)
{
  logsvc::client::DefaultConnection connection = create_connection();
  DummyDeliverable dummy_deliverable;
  connection.send(dummy_deliverable);
  BOOST_CHECK_EQUAL(12, dummy_socket->async_read_byte_count);
}

BOOST_FIXTURE_TEST_CASE(after_receiving_header_tries_to_read_payload, F)
{
  logsvc::client::DefaultConnection connection = create_connection();
  DummyDeliverable dummy_deliverable;
  connection.send(dummy_deliverable);
  const std::string header("123456789012");
  drf->expected_payload = "Test";
  dummy_socket->receive_bytes(header);
  BOOST_CHECK_EQUAL(header, drf->received_bytes);
  BOOST_CHECK_EQUAL(drf->expected_payload.size(), dummy_socket->async_read_byte_count);
  dummy_socket->receive_bytes(drf->expected_payload);
}

BOOST_FIXTURE_TEST_CASE(tries_to_read_new_message_after_next_send, F)
{
  logsvc::client::DefaultConnection connection = create_connection();
  DummyDeliverable dummy_deliverable;
  connection.send(dummy_deliverable);
  const std::string header("123456789012");
  drf->expected_payload = "Test";
  dummy_socket->receive_bytes(header);
  dummy_socket->receive_bytes(drf->expected_payload);

  connection.send(dummy_deliverable);
  drf->expected_payload = "foobar";
  dummy_socket->receive_bytes(header);

  BOOST_CHECK_EQUAL(drf->expected_payload.size(), dummy_socket->async_read_byte_count);
  dummy_socket->receive_bytes("foobar");
}

BOOST_FIXTURE_TEST_CASE(future_value_set_when_payload_received, F)
{
  logsvc::client::DefaultConnection connection = create_connection();
  DummyDeliverable dummy_deliverable;
  std::future<std::unique_ptr<logsvc::prot::Receivable>> future_recv
    = connection.send(dummy_deliverable);
  BOOST_CHECK(std::future_status::timeout
              == future_recv.wait_for(std::chrono::microseconds(0)));

  const std::string header("123456789012");
  drf->expected_payload = "Test";
  dummy_socket->receive_bytes(header);
  BOOST_CHECK(std::future_status::timeout
              == future_recv.wait_for(std::chrono::microseconds(0)));

  dummy_socket->receive_bytes(drf->expected_payload);
  BOOST_CHECK(std::future_status::ready
              == future_recv.wait_for(std::chrono::microseconds(0)));
}

BOOST_FIXTURE_TEST_CASE(gets_expected_receivable, F)
{
  logsvc::client::DefaultConnection connection = create_connection();
  DummyDeliverable dummy_deliverable;
  std::future<std::unique_ptr<logsvc::prot::Receivable>> future_recv
    = connection.send(dummy_deliverable);
  const std::string header("123456789012");
  drf->expected_payload = "Test";
  dummy_socket->receive_bytes(header);
  dummy_socket->receive_bytes(drf->expected_payload);
  std::unique_ptr<logsvc::prot::Receivable> recv = future_recv.get();
  BOOST_REQUIRE(recv != nullptr);
  mock::DummyExecutor exec;
  recv->act(exec);
  std::string message = egen::lookup(logsvc::prot::FileHandle(0x42),
                                     exec.messages,
                                     std::string("Incorrect receivable."));
  BOOST_CHECK_EQUAL("Hello", message);
}

BOOST_FIXTURE_TEST_CASE(if_error_occurs_exception_is_thrown_from_future, F)
{
  logsvc::client::DefaultConnection connection = create_connection();
  DummyDeliverable dummy_deliverable;
  std::future<std::unique_ptr<logsvc::prot::Receivable>> future_recv
    = connection.send(dummy_deliverable);
  const std::string error("the particular error message");
  dummy_socket->make_error_occur(error);
  BOOST_REQUIRE(std::future_status::ready
                == future_recv.wait_for(std::chrono::microseconds(0)));
  BOOST_CHECK_EXCEPTION(future_recv.get(),
                        logsvc::SocketError,
                        [&](const logsvc::SocketError& e) -> bool
                        {
                          const std::string what(e.what());
                          return what.find(error) != std::string::npos;
                        });
}

BOOST_FIXTURE_TEST_CASE(behaves_correctly_if_error_occurs, F)
{
  logsvc::client::DefaultConnection connection = create_connection();
  DummyDeliverable dummy_deliverable;
  std::future<std::unique_ptr<logsvc::prot::Receivable>> future_recv
    = connection.send(dummy_deliverable);
  const std::string error("the particular error message");
  dummy_socket->make_error_occur(error);
  future_recv = connection.send(dummy_deliverable);
  dummy_socket->receive_bytes("foobarfoobar");
  dummy_socket->make_error_occur(error);
  future_recv = connection.send(dummy_deliverable);
  drf->expected_payload = "foo";
  dummy_socket->receive_bytes("foobarfoobar");
  dummy_socket->receive_bytes("foo");
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
