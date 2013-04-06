/* Source file created: 2013-04-06

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

#include "log/Client.h"
#include "log/File.h"
#include "log/FileHandle.h"
#include "log/Message.h"
#include "log/ProtObjFactory.h"
#include "log/ReceivableFactory.h"

using namespace logsvc::prot;

BOOST_AUTO_TEST_SUITE(testProtObjFactory)

struct F
{
  F() : factory() {}
  ~F() {}
  ProtObjFactory factory;
};

BOOST_FIXTURE_TEST_CASE(is_a_ReceivableFactory, F)
{
  BOOST_CHECK(dynamic_cast<ReceivableFactory*>(&factory) != nullptr);
}

// [X] "open" # open file, prot::File, return prot::FileHandle, nack
// [ ] "clos" # close file, prot::FileHandle, return ackn, nack
// [X] "clnt" # client description, prot::Client, return ackn, nack
// [X] "mesg" # message to write to open log file, prot::Message, return ackn, nack
// [X] "filh" # Handle to open file, prot::FileHandle
// [ ] "clnh" # Handle for client to present, prot::ClientHandle
// [ ] "ackn" # Acknowledged, prot::Ack
// [ ] "nack" # Not Acknowledged, prot::Nack


BOOST_FIXTURE_TEST_CASE(can_create_File, F)
{
  std::unique_ptr<Receivable> receivable =
    factory.create(std::string("logsopen\xa\0\0\0", 12));
  BOOST_REQUIRE(receivable != nullptr);
  BOOST_CHECK(dynamic_cast<File*>(receivable.get()) != nullptr);
  BOOST_CHECK_EQUAL(10, receivable->get_payload_length());
}

BOOST_FIXTURE_TEST_CASE(can_create_Client, F)
{
  std::unique_ptr<Receivable> receivable =
    factory.create(std::string("logsclnt\xb\0\0\0", 12));
  BOOST_REQUIRE(receivable != nullptr);
  BOOST_CHECK(dynamic_cast<Client*>(receivable.get()) != nullptr);
  BOOST_CHECK_EQUAL(11, receivable->get_payload_length());
}

BOOST_FIXTURE_TEST_CASE(can_create_Message, F)
{
  std::unique_ptr<Receivable> receivable =
    factory.create(std::string("logsmesg\x1c\0\0\0", 12));
  BOOST_REQUIRE(receivable != nullptr);
  BOOST_CHECK(dynamic_cast<Message*>(receivable.get()) != nullptr);
  BOOST_CHECK_EQUAL(28, receivable->get_payload_length());
}

BOOST_FIXTURE_TEST_CASE(can_create_FileHandle, F)
{
  std::unique_ptr<Receivable> receivable =
    factory.create(std::string("logsfilh\x04\0\0\0", 12));
  BOOST_REQUIRE(receivable != nullptr);
  BOOST_CHECK(dynamic_cast<FileHandle*>(receivable.get()) != nullptr);
  BOOST_CHECK_EQUAL(4, receivable->get_payload_length());
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
