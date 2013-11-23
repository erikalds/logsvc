/* Source file created: 2013-11-17

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
#include "log/Acknowledged.h"
#include "log/ClientExecutor.h"
#include "log/ClientHandle.h"
#include "log/Disconnect.h"
#include "log/FileHandle.h"
#include "log/NotAcknowledged.h"
#include "log/Receivable.h"

BOOST_AUTO_TEST_SUITE(testDisconnect)

struct F
{
  F() {}
  ~F() {}
};

BOOST_FIXTURE_TEST_CASE(is_a_receivable, F)
{
  logsvc::prot::Disconnect disconnect;
  BOOST_CHECK(dynamic_cast<logsvc::prot::Receivable*>(&disconnect));
}

BOOST_FIXTURE_TEST_CASE(has_correct_payload_length, F)
{
  logsvc::prot::ClientHandle client_handle(42);
  logsvc::prot::Disconnect disconnect(client_handle);
  BOOST_CHECK_EQUAL(client_handle.get_payload_length(),
                    disconnect.get_payload_length());

  logsvc::prot::Disconnect receiving_disconnect;
  BOOST_CHECK_EQUAL(client_handle.get_payload_length(),
                    receiving_disconnect.get_payload_length());
}

BOOST_FIXTURE_TEST_CASE(is_final_message, F)
{
  logsvc::prot::Disconnect disconnect;
  BOOST_CHECK(disconnect.is_final_message());
}

namespace
{

  class MyClientExecutor : public logsvc::prot::ClientExecutor
  {
  public:
    MyClientExecutor() : actions(0) {}

    void set_file_handle(const logsvc::prot::FileHandle&) override { ++actions; }
    void set_error(const std::string& msg) override { ++actions; error = msg; }
    void success() override { ++actions; }
    void set_client_handle(const logsvc::prot::ClientHandle&) override {}

    unsigned int actions;
    std::string error;
  };
} // anonymous namespace

BOOST_FIXTURE_TEST_CASE(does_not_act_on_ClientExecutor, F)
{
  MyClientExecutor client_exec;
  logsvc::prot::Disconnect disconnect;
  disconnect.act(client_exec);
  BOOST_CHECK_EQUAL(0, client_exec.actions);
}

BOOST_FIXTURE_TEST_CASE(act_on_Executor_calls_disconnect_client, F)
{
  mock::DummyExecutor exec;
  logsvc::prot::ClientHandle client = exec.set_client_info("asdf", "127.0.0.1");
  logsvc::prot::Disconnect disconnect(client);
  disconnect.act(exec);
  BOOST_CHECK_EQUAL("DISCONNECTED", exec.client_name);
}

BOOST_FIXTURE_TEST_CASE(act_on_Executor_Acknowledges_correct_client_handle, F)
{
  mock::DummyExecutor exec;
  exec.client_handle = logsvc::prot::ClientHandle(12345678);
  logsvc::prot::ClientHandle client = exec.set_client_info("asdf", "127.0.0.1");
  logsvc::prot::Disconnect disconnect(client);
  std::unique_ptr<logsvc::prot::Deliverable> deliverable = disconnect.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK(dynamic_cast<logsvc::prot::Acknowledged*>(deliverable.get())
              != nullptr);
}

BOOST_FIXTURE_TEST_CASE(act_on_Executor_fails_on_incorrect_client_handle, F)
{
  mock::DummyExecutor exec;
  exec.error_msg_on_wrong_client_handle_in_disconnect = "Some error";
  logsvc::prot::ClientHandle client(42424242);
  logsvc::prot::Disconnect disconnect(client);
  std::unique_ptr<logsvc::prot::Deliverable> deliverable = disconnect.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  logsvc::prot::NotAcknowledged* nack
    = dynamic_cast<logsvc::prot::NotAcknowledged*>(deliverable.get());
  BOOST_REQUIRE(nack != nullptr);

  MyClientExecutor client_exec;
  nack->act(client_exec);
  BOOST_CHECK_EQUAL(exec.error_msg_on_wrong_client_handle_in_disconnect,
                    client_exec.error);
}

BOOST_FIXTURE_TEST_CASE(reads_client_handle_payload, F)
{
  mock::DummyExecutor exec;
  exec.client_handle = logsvc::prot::ClientHandle(98765432);
  logsvc::prot::ClientHandle client = exec.set_client_info("client_name",
                                                           "127.0.0.1");
  logsvc::prot::Disconnect disconnect;
  disconnect.read_payload(client.get_payload());
  disconnect.act(exec);
  BOOST_CHECK_EQUAL("DISCONNECTED", exec.client_name);
}

BOOST_FIXTURE_TEST_CASE(is_a_Deliverable, F)
{
  logsvc::prot::Disconnect disconnect;
  BOOST_CHECK(dynamic_cast<logsvc::prot::Deliverable*>(&disconnect));
}

BOOST_FIXTURE_TEST_CASE(get_correct_header, F)
{
  logsvc::prot::Disconnect disconnect;
  BOOST_CHECK_EQUAL(std::string("logsdisc\x04\0\0\0", 12),
                    disconnect.get_header());
}

BOOST_FIXTURE_TEST_CASE(get_correct_payload, F)
{
  logsvc::prot::ClientHandle ch0(0x1234);
  logsvc::prot::Disconnect disconnect0(ch0);
  BOOST_CHECK_EQUAL(ch0.get_payload(),
                    disconnect0.get_payload());
  logsvc::prot::ClientHandle ch1(0x12345678);
  logsvc::prot::Disconnect disconnect1(ch1);
  BOOST_CHECK_EQUAL(ch1.get_payload(),
                    disconnect1.get_payload());
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
