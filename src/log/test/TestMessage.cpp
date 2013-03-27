/* Source file created: 2013-03-27

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
#include "log/FileHandle.h"
#include "log/Message.h"
#include "log/test/DummyExecutor.h"
#include <egen/lookup.h>

BOOST_AUTO_TEST_SUITE(testMessage)

struct F
{
  F() : msg0("message", logsvc::prot::FileHandle(0x1234)),
        msg1("another message", logsvc::prot::FileHandle(0x2345)) {}
  ~F() {}

  logsvc::prot::Message msg0, msg1;
};

BOOST_FIXTURE_TEST_CASE(can_get_message, F)
{
  BOOST_CHECK_EQUAL("message", msg0.get_message());
  BOOST_CHECK_EQUAL("another message", msg1.get_message());
}

BOOST_FIXTURE_TEST_CASE(can_get_filehandle, F)
{
  BOOST_CHECK(logsvc::prot::FileHandle(0x1234) == msg0.get_filehandle());
  BOOST_CHECK(logsvc::prot::FileHandle(0x2345) == msg1.get_filehandle());
}

BOOST_FIXTURE_TEST_CASE(can_read_payload, F)
{
  logsvc::prot::Message msg;
  msg.read_payload(std::string("\x56\x34\0\0a message", 13));
  BOOST_CHECK_EQUAL("a message", msg.get_message());
  BOOST_CHECK(logsvc::prot::FileHandle(0x3456) == msg.get_filehandle());

  msg.read_payload(std::string("\x67\x45\0\0stuff", 9));
  BOOST_CHECK_EQUAL("stuff", msg.get_message());
  BOOST_CHECK(logsvc::prot::FileHandle(0x4567) == msg.get_filehandle());
}

BOOST_FIXTURE_TEST_CASE(act_writes_message_with_filehandle, F)
{
  mock::DummyExecutor exec;
  msg0.act(exec);
  BOOST_CHECK_EQUAL(egen::lookup(logsvc::prot::FileHandle(0x1234),
                                 exec.messages, std::string("not written")),
                    "message");
  msg1.act(exec);
  BOOST_CHECK_EQUAL(egen::lookup(logsvc::prot::FileHandle(0x2345),
                                 exec.messages, std::string("not written")),
                    "another message");
}

BOOST_FIXTURE_TEST_CASE(act_positive, F)
{
  mock::DummyExecutor exec;
  std::unique_ptr<logsvc::prot::Deliverable> deliverable = msg0.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsackn\0\0\0\0", 12), deliverable->get_header());
}

BOOST_FIXTURE_TEST_CASE(act_negative, F)
{
  mock::DummyExecutor exec;
  exec.write_message_fails = true;
  std::unique_ptr<logsvc::prot::Deliverable> deliverable = msg0.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsnack\5\0\0\0", 12), deliverable->get_header());
  BOOST_CHECK_EQUAL("error", deliverable->get_payload());

  exec.error_string = "fail";
  deliverable = msg0.act(exec);
  BOOST_REQUIRE(deliverable != nullptr);
  BOOST_CHECK_EQUAL(std::string("logsnack\4\0\0\0", 12), deliverable->get_header());
  BOOST_CHECK_EQUAL("fail", deliverable->get_payload());
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
