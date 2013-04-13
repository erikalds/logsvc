/* Source file created: 2013-03-25

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

#include "log/test/DummyExecutor.h"
#include "logsvc_daemon/test/DummySocket.h"
#include "logsvc_daemon/Session.h"
#include "logsvc_daemon/DefaultSocketSession.h"
#include "log/FileHandle.h"
#include "log/NotAcknowledged.h"
#include "log/Receivable.h"
#include "log/ReceivableFactory.h"
#include <egen/lookup.h>

BOOST_AUTO_TEST_SUITE(testDefaultSocketSession)

class DummyReceivable : public logsvc::prot::Receivable
{
public:
  DummyReceivable(const std::string& expected_payload) :
    expected_payload(expected_payload), received_payload(false) {}
  ~DummyReceivable()
  {
    if (!expected_payload.empty())
      BOOST_CHECK(received_payload == true);
  }

  virtual std::size_t get_payload_length() const
  {
    return expected_payload.size();
  }

  virtual void read_payload(const std::string& payload)
  {
    BOOST_CHECK_EQUAL(expected_payload, payload);
    received_payload = true;
  }
  virtual std::unique_ptr<logsvc::prot::Deliverable> act(logsvc::prot::Executor& exec)
  {
    exec.write_message(logsvc::prot::FileHandle(0x42), "Hello");
    return std::unique_ptr<logsvc::prot::Deliverable>(new logsvc::prot::NotAcknowledged("fail"));
  }

private:
  std::string expected_payload;
  bool received_payload;
};

class DummyReceivableFactory : public logsvc::prot::ReceivableFactory
{
public:
  std::unique_ptr<logsvc::prot::Receivable> create(const std::string& header)
  {
    received_bytes += header;
    return std::unique_ptr<logsvc::prot::Receivable>(new DummyReceivable(expected_payload));
  }

  std::string received_bytes;
  std::string expected_payload;
};

struct F
{
  F() : socket(), exec(), drf() {}
  ~F() {}

  mock::DummySocket socket;
  mock::DummyExecutor exec;
  DummyReceivableFactory drf;
};

BOOST_FIXTURE_TEST_CASE(canCreate, F)
{
  logsvc::daemon::DefaultSocketSession ss(socket, exec, drf);
}

BOOST_FIXTURE_TEST_CASE(createdSession_startsListeningToSocket, F)
{
  logsvc::daemon::DefaultSocketSession ss(socket, exec, drf);
  BOOST_REQUIRE_EQUAL(0, socket.async_read_call_count);
  ss.start_listen();
  BOOST_CHECK_EQUAL(1, socket.async_read_call_count);
  BOOST_CHECK_EQUAL(12, socket.async_read_byte_count);
}

BOOST_FIXTURE_TEST_CASE(receive_bytes_sends_bytes_to_ReceivableFactory, F)
{
  logsvc::daemon::DefaultSocketSession ss(socket, exec, drf);
  ss.start_listen();
  std::string header("logsmesg\x09\0\0\0", 12);
  socket.receive_bytes(header);
  BOOST_CHECK_EQUAL(header, drf.received_bytes);
}

BOOST_FIXTURE_TEST_CASE(received_bytes_after_header_sends_bytes_to_Receivable, F)
{
  logsvc::daemon::DefaultSocketSession ss(socket, exec, drf);
  ss.start_listen();
  std::string header("logsmesg\x09\0\0\0", 12);
  std::string payload("\x42\0\0\0Hello", 9);
  drf.expected_payload = payload;

  socket.receive_bytes(header);
  BOOST_CHECK_EQUAL(2, socket.async_read_call_count);
  BOOST_CHECK_EQUAL(9, socket.async_read_byte_count);
  socket.receive_bytes(payload);
}

BOOST_FIXTURE_TEST_CASE(after_bytes_sent_to_Receivable_it_is_allowed_to_act, F)
{
  logsvc::daemon::DefaultSocketSession ss(socket, exec, drf);
  ss.start_listen();
  std::string header("logsmesg\x09\0\0\0", 12);
  std::string payload("\x42\0\0\0Hello", 9);
  drf.expected_payload = payload;

  socket.receive_bytes(header);
  socket.receive_bytes(payload);

  BOOST_CHECK_EQUAL(egen::lookup(logsvc::prot::FileHandle(0x42), exec.messages,
                                 std::string("file not written")),
                    "Hello");
}

BOOST_FIXTURE_TEST_CASE(after_it_has_acted_the_Deliverable_is_written_to_the_Socket, F)
{
  logsvc::daemon::DefaultSocketSession ss(socket, exec, drf);
  ss.start_listen();
  std::string header("logsmesg\x09\0\0\0", 12);
  std::string payload("\x42\0\0\0Hello", 9);
  drf.expected_payload = payload;

  socket.receive_bytes(header);
  socket.receive_bytes(payload);

  BOOST_CHECK_EQUAL(socket.written_bytes, std::string("logsnack\4\0\0\0fail", 16));
}

BOOST_FIXTURE_TEST_CASE(can_read_several_protocol_objects, F)
{
  logsvc::daemon::DefaultSocketSession ss(socket, exec, drf);
  ss.start_listen();
  std::string header0("logsmesg\x09\0\0\0", 12);
  std::string payload0("\x42\0\0\0Hello", 9);
  drf.expected_payload = payload0;

  socket.receive_bytes(header0);
  socket.receive_bytes(payload0);

  std::string header1("logsopen\x10\0\0\0", 12);
  std::string payload1("path/to/file.txt");
  drf.expected_payload = payload1;

  socket.receive_bytes(header1);
  BOOST_CHECK_EQUAL(16, socket.async_read_byte_count);
  socket.receive_bytes(payload1);
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
