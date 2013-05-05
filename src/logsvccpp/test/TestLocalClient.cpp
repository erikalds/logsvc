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
#include "log/ClientHandle.h"
#include "log/Deliverable.h"
#include "log/Executor.h"
#include "log/FileHandle.h"
#include "log/ProtObjFactory.h"

#include <set>

BOOST_AUTO_TEST_SUITE(testClientHost)

class DummySessionConnection;

class DSCKilledListener
{
public:
  virtual ~DSCKilledListener() {}

  virtual void connection_killed(DummySessionConnection*) const = 0;
};

class DummySessionConnection : public logsvc::client::SessionConnection,
                               public logsvc::prot::Executor
{
public:
  DummySessionConnection(const DSCKilledListener* listener) : client_name("UNSET"),
                                                              client_address("UNSET"),
                                                              listener(listener) {}
  ~DummySessionConnection() { listener->connection_killed(this); }

  virtual void send(const logsvc::prot::Deliverable& deliverable)
  {
    logsvc::prot::ProtObjFactory factory;
    std::unique_ptr<logsvc::prot::Receivable> recv =
      factory.create(deliverable.get_header());
    BOOST_REQUIRE(recv != nullptr);
    recv->read_payload(deliverable.get_payload());
    recv->act(*this);
  }

  virtual logsvc::prot::FileHandle open_file(const boost::filesystem::path& filename)
  { return logsvc::prot::FileHandle(); }
  virtual void close_file(const logsvc::prot::FileHandle& fh) {}
  virtual void write_message(const logsvc::prot::FileHandle& fh,
                             const std::string& message)
  {}
  virtual logsvc::prot::ClientHandle set_client_info(const std::string& name,
                                                     const std::string& address)
  {
    client_name = name;
    client_address = address;
    return logsvc::prot::ClientHandle();
  }

  std::string client_name;
  std::string client_address;

private:
  const DSCKilledListener* listener;
};

class DummyConnectionFactory : public logsvc::client::ConnectionFactory,
                               public DSCKilledListener
{
public:
  DummyConnectionFactory() : create_count(0), live_ptrs() {}

  virtual std::unique_ptr<logsvc::client::SessionConnection> create_session() const
  {
    ++create_count;
    DummySessionConnection* conn = new DummySessionConnection(this);
    live_ptrs.insert(conn);
    return std::unique_ptr<logsvc::client::SessionConnection>(conn);
  }

  virtual void connection_killed(DummySessionConnection* conn) const
  {
    live_ptrs.erase(conn);
  }

  std::size_t live_count() const { return live_ptrs.size(); }

  mutable int create_count;
  mutable std::set<DummySessionConnection*> live_ptrs;
};

struct F
{
  F() : connection_factory(), host() { create_host("appname"); }
  ~F() {}

  std::string get_set_client_name() const
  {
    BOOST_REQUIRE_EQUAL(connection_factory.live_ptrs.size(), 1);
    DummySessionConnection* conn = *connection_factory.live_ptrs.begin();
    BOOST_REQUIRE(conn != nullptr);
    return conn->client_name;
  }

  void create_host(const std::string& appname)
  { host.reset(new logsvc::client::Host(appname, connection_factory)); }

  DummyConnectionFactory connection_factory;
  std::unique_ptr<logsvc::client::Host> host;
};

BOOST_FIXTURE_TEST_CASE(constructor_creates_session_connection, F)
{
  BOOST_CHECK_EQUAL(1, connection_factory.create_count);
}

BOOST_FIXTURE_TEST_CASE(keeps_session_connection, F)
{
  BOOST_CHECK_EQUAL(1, connection_factory.live_count());
}

BOOST_FIXTURE_TEST_CASE(session_connection_dies_with_host, F)
{
  host.reset();
  BOOST_CHECK_EQUAL(0, connection_factory.live_count());
}

BOOST_FIXTURE_TEST_CASE(sends_prot_client_via_session_connection, F)
{
  BOOST_CHECK_EQUAL(get_set_client_name(), "appname");
  create_host("another_appname");
  BOOST_CHECK_EQUAL(get_set_client_name(), "another_appname");
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
