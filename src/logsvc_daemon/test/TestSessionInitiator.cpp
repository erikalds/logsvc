/* Source file created: 2013-04-13

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

#include "logsvc_daemon/test/DummySocket.h"
#include "logsvc_daemon/SessionInitiator.h"
#include "logsvc_daemon/SocketSession.h"
#include "logsvc_daemon/SocketSessionFactory.h"
#include "network/SocketAcceptor.h"
#include "network/SocketAcceptListener.h"

using namespace logsvc::daemon;

BOOST_AUTO_TEST_SUITE(testSessionInitiator)

class DummySocketSession;

class DummySocketDeleteListener
{
public:
  virtual ~DummySocketDeleteListener() {}
  virtual void deleting(DummySocketSession* session) = 0;
};

class DummySocketSession : public SocketSession
{
public:
  DummySocketSession(DummySocketDeleteListener* listener) :
    is_listening(false), delete_listener(listener) {}
  ~DummySocketSession()
  { delete_listener->deleting(this); }

  virtual void start_listen() { is_listening = true; }

  bool is_listening;
  DummySocketDeleteListener* delete_listener;
};

class DummySocketSessionFactory : public SocketSessionFactory,
                                  public DummySocketDeleteListener
{
public:
  DummySocketSessionFactory() : create_count(0), last_socket(), last_session(nullptr) {}

  virtual std::unique_ptr<SocketSession>
  create_session(std::unique_ptr<network::Socket> socket)
  {
    ++create_count;
    last_socket = std::move(socket);
    std::unique_ptr<DummySocketSession> session(new DummySocketSession(this));
    last_session = session.get();
    return std::move(session);
  }

  virtual void deleting(DummySocketSession* session)
  {
    if (last_session == session)
      last_session = nullptr;
  }

  int create_count;
  std::unique_ptr<network::Socket> last_socket;
  DummySocketSession* last_session;
};

class DummySocketAcceptor : public network::SocketAcceptor
{
public:
  DummySocketAcceptor() : current_listener(nullptr) {}

  virtual void async_accept(network::SocketAcceptListener& listener)
  {
    current_listener = &listener;
  }

  void request_accept()
  {
    BOOST_REQUIRE(current_listener != nullptr);
    std::unique_ptr<network::Socket> dummysocket(new mock::DummySocket);
    last_socket = dummysocket.get();
    current_listener->accept_requested(std::move(dummysocket));
    current_listener = nullptr;
  }

  network::SocketAcceptListener* current_listener;
  network::Socket* last_socket;
};

struct F
{
  F() {}
  ~F() {}
};

BOOST_FIXTURE_TEST_CASE(creates_socket_session_on_request, F)
{
  DummySocketSessionFactory factory;
  DummySocketAcceptor acceptor;
  SessionInitiator initiator(factory, acceptor);
  acceptor.request_accept();
  BOOST_CHECK_EQUAL(1, factory.create_count);
}

BOOST_FIXTURE_TEST_CASE(passes_on_socket, F)
{
  DummySocketSessionFactory factory;
  DummySocketAcceptor acceptor;
  SessionInitiator initiator(factory, acceptor);
  acceptor.request_accept();
  BOOST_CHECK_EQUAL(acceptor.last_socket, factory.last_socket.get());
}

BOOST_FIXTURE_TEST_CASE(calls_start_listen, F)
{
  DummySocketSessionFactory factory;
  DummySocketAcceptor acceptor;
  SessionInitiator initiator(factory, acceptor);
  acceptor.request_accept();
  BOOST_REQUIRE(factory.last_session != nullptr);
  BOOST_CHECK(factory.last_session->is_listening);
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
