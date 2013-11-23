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

#include "logsvccpp/test/DummySessionConnection.h"
#include "logsvccpp/client/LocalClient.h"
#include "logsvccpp/client/ConnectionFactory.h"
#include "logsvccpp/client/RemoteLogFile.h"
#include "logsvccpp/UnableToConnectError.h"
#include "log/Client.h"

#include <set>

BOOST_AUTO_TEST_SUITE(testLocalClient)

class DummyConnectionFactory : public logsvc::client::ConnectionFactory,
                               public logsvc::mock::DSCKilledListener
{
public:
  DummyConnectionFactory() : create_count(0), live_ptrs(), unable_to_connect(false),
                             address("192.168.1.102") {}

  virtual std::unique_ptr<logsvc::client::SessionConnection> create_session() const
  {
    if (unable_to_connect)
      return std::unique_ptr<logsvc::client::SessionConnection>();

    ++create_count;
    logsvc::mock::DummySessionConnection* conn
      = new logsvc::mock::DummySessionConnection(this);
    conn->client_handle = client_handle;
    conn->set_client_info_error = set_client_info_error;
    live_ptrs.insert(conn);
    return std::unique_ptr<logsvc::client::SessionConnection>(conn);
  }

  virtual logsvc::prot::Client create_client_info(const std::string& appname) const
  {
    return logsvc::prot::Client(appname,
                                boost::asio::ip::address::from_string(address));
  }

  virtual void connection_killed(logsvc::mock::DummySessionConnection* conn) const
  {
    live_ptrs.erase(conn);
    was_disconnected_before_killed = conn->disconnected;
  }

  std::size_t live_count() const { return live_ptrs.size(); }

  mutable int create_count;
  mutable std::set<logsvc::mock::DummySessionConnection*> live_ptrs;
  bool unable_to_connect;
  std::string address;
  mutable bool was_disconnected_before_killed;
  logsvc::prot::ClientHandle client_handle;
  std::string set_client_info_error;
};

struct F
{
  F() : connection_factory(), local_client() { create_local_client("appname"); }
  ~F() {}

  logsvc::mock::DummySessionConnection* get_live_connection() const
  {
    BOOST_REQUIRE_EQUAL(connection_factory.live_ptrs.size(), 1);
    logsvc::mock::DummySessionConnection* conn =
      *connection_factory.live_ptrs.begin();
    BOOST_REQUIRE(conn != nullptr);
    return conn;
  }

  std::string get_set_client_name() const
  {
    return get_live_connection()->client_name;
  }

  std::string get_set_client_address() const
  {
    return get_live_connection()->client_address;
  }

  std::vector<boost::filesystem::path> get_opened_files() const
  {
    return get_live_connection()->opened_files;
  }

  void create_local_client(const std::string& appname)
  { local_client.reset(new logsvc::client::LocalClient(appname, connection_factory)); }

  DummyConnectionFactory connection_factory;
  std::unique_ptr<logsvc::client::LocalClient> local_client;
};

BOOST_FIXTURE_TEST_CASE(constructor_creates_session_connection, F)
{
  BOOST_CHECK_EQUAL(1, connection_factory.create_count);
}

BOOST_FIXTURE_TEST_CASE(keeps_session_connection, F)
{
  BOOST_CHECK_EQUAL(1, connection_factory.live_count());
}

BOOST_FIXTURE_TEST_CASE(session_connection_dies_with_local_client, F)
{
  local_client.reset();
  BOOST_CHECK_EQUAL(0, connection_factory.live_count());
}

BOOST_FIXTURE_TEST_CASE(sends_prot_client_via_session_connection, F)
{
  BOOST_CHECK_EQUAL(get_set_client_name(), "appname");
  create_local_client("another_appname");
  BOOST_CHECK_EQUAL(get_set_client_name(), "another_appname");
}

BOOST_FIXTURE_TEST_CASE(throws_UnableToConnectError_when_ConnectionFactory_creates_nullptr,
                        F)
{
  connection_factory.unable_to_connect = true;
  BOOST_CHECK_THROW(create_local_client("appname"), logsvc::UnableToConnectError);
}

BOOST_FIXTURE_TEST_CASE(sends_ip_from_connection_factory, F)
{
  BOOST_CHECK_EQUAL(get_set_client_address(), connection_factory.address);
  connection_factory.address = "10.30.30.250";
  create_local_client("appname");
  BOOST_CHECK_EQUAL(get_set_client_address(), connection_factory.address);
}

BOOST_FIXTURE_TEST_CASE(open_log_file, F)
{
  std::unique_ptr<logsvc::client::RemoteLogFile> remote_file0
    = local_client->open_remote_file("asdf.txt");
  BOOST_CHECK(remote_file0 != nullptr);
  std::vector<boost::filesystem::path> opened_files = get_opened_files();
  BOOST_REQUIRE_EQUAL(1, opened_files.size());
  BOOST_CHECK_EQUAL("asdf.txt", opened_files[0]);

  std::unique_ptr<logsvc::client::RemoteLogFile> remote_file1
    = local_client->open_remote_file("foobar.txt");
  opened_files = get_opened_files();
  BOOST_REQUIRE_EQUAL(2, opened_files.size());
  BOOST_CHECK_EQUAL("foobar.txt", opened_files[1]);
}

BOOST_FIXTURE_TEST_CASE(disconnects_when_destroyed, F)
{
  connection_factory.client_handle = logsvc::prot::ClientHandle(23456);
  create_local_client("appname");
  BOOST_CHECK(!get_live_connection()->disconnected);
  local_client.reset();
  BOOST_CHECK(connection_factory.was_disconnected_before_killed);
}

BOOST_FIXTURE_TEST_CASE(Throws_UnableToConnectError_if_sending_client_info_fails,
                        F)
{
  connection_factory.set_client_info_error = "The set_client_info error.";
  BOOST_CHECK_EXCEPTION(create_local_client("appname"),
                        logsvc::UnableToConnectError,
                        [&](const logsvc::UnableToConnectError& utce)
                        {
                          const std::string estr =
                            connection_factory.set_client_info_error;
                          return std::string(utce.what()).find(estr)
                            != std::string::npos;
                        });
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
