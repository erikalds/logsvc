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

#include "log/Acknowledged.h"
#include "log/Client.h"
#include "log/ClientHandle.h"
#include "log/CloseFile.h"
#include "log/Disconnect.h"
#include "log/File.h"
#include "log/FileHandle.h"
#include "log/MalformedHeader.h"
#include "log/Message.h"
#include "log/NotAcknowledged.h"
#include "log/ProtObjFactory.h"
#include "log/ReceivableFactory.h"
#include "log/UnknownProtocolObjectType.h"
#include <boost/regex.hpp>

using namespace logsvc::prot;

BOOST_AUTO_TEST_SUITE(testProtObjFactory)

struct F
{
  F() : factory() {}
  ~F() {}

  ProtObjFactory factory;

  template<typename ReceivableSubClass>
  void can_create_(const std::string& type, std::size_t payload_length)
  {
    std::string encoded_pl("\0\0\0\0", 4);
    for (int i = 0; i < 4; ++i)
      encoded_pl[i] = static_cast<unsigned char>((payload_length >> (i * 8)) & 0xff);
    std::unique_ptr<Receivable> receivable =
      factory.create("logs" + type + encoded_pl);
    BOOST_REQUIRE(receivable != nullptr);
    BOOST_CHECK(dynamic_cast<ReceivableSubClass*>(receivable.get()) != nullptr);
    BOOST_CHECK_EQUAL(payload_length, receivable->get_payload_length());
  }
};

BOOST_FIXTURE_TEST_CASE(is_a_ReceivableFactory, F)
{
  BOOST_CHECK(dynamic_cast<ReceivableFactory*>(&factory) != nullptr);
}

// [X] "open" # open file, prot::File, return prot::FileHandle, nack
// [X] "clos" # close file, prot::FileHandle, return ackn, nack
// [X] "clnt" # client description, prot::Client, return ackn, nack
// [X] "mesg" # message to write to open log file, prot::Message, return ackn, nack
// [X] "filh" # Handle to open file, prot::FileHandle
// [X] "clnh" # Handle for client to present, prot::ClientHandle
// [X] "ackn" # Acknowledged, prot::Ack
// [X] "nack" # Not Acknowledged, prot::Nack
// [X] "disc" # Disconnect, prot::Disconnect


BOOST_FIXTURE_TEST_CASE(can_create_File, F)
{
  can_create_<File>("open", 10);
}

BOOST_FIXTURE_TEST_CASE(can_create_Client, F)
{
  can_create_<Client>("clnt", 11);
}

BOOST_FIXTURE_TEST_CASE(can_create_Message, F)
{
  can_create_<Message>("mesg", 28);
}

BOOST_FIXTURE_TEST_CASE(can_create_FileHandle, F)
{
  can_create_<FileHandle>("filh", 4);
}

BOOST_FIXTURE_TEST_CASE(can_create_ClientHandle, F)
{
  can_create_<ClientHandle>("clnh", 4);
}

BOOST_FIXTURE_TEST_CASE(can_create_Acknowledged, F)
{
  can_create_<Acknowledged>("ackn", 0);
}

BOOST_FIXTURE_TEST_CASE(can_create_NotAcknowledged, F)
{
  can_create_<NotAcknowledged>("nack", 42);
}

BOOST_FIXTURE_TEST_CASE(can_create_CloseFile, F)
{
  can_create_<CloseFile>("clos", 4);
}

BOOST_FIXTURE_TEST_CASE(can_create_Disconnect, F)
{
  can_create_<Disconnect>("disc", 4);
}

BOOST_FIXTURE_TEST_CASE(error_on_unknown_type, F)
{
  BOOST_CHECK_EXCEPTION(factory.create(std::string("logsasdf\0\0\0\0", 12)),
                        UnknownProtocolObjectType,
                        [](const UnknownProtocolObjectType& e)
                        {
                          return boost::regex_search(e.what(),
                                                     boost::regex("[Uu]nknown protocol object type"))
                            && boost::regex_search(e.what(), boost::regex("\"asdf\""));
                        });
}

BOOST_FIXTURE_TEST_CASE(error_on_not_allowed_payload_length, F)
{
  BOOST_CHECK_EXCEPTION(factory.create(std::string("logsackn\1\0\0\0", 12)),
                        MalformedHeader,
                        [](const MalformedHeader& e)
                        {
                          return boost::regex_search(e.what(),
                                                     boost::regex("[Ii]ncorrect payload length"))
                            && boost::regex_search(e.what(), boost::regex("command \"ackn\""))
                            && boost::regex_search(e.what(), boost::regex("\\b1\\b"))
                            && boost::regex_search(e.what(), boost::regex("\\b0\\b"));
                        });
  BOOST_CHECK_EXCEPTION(factory.create(std::string("logsfilh\1\0\0\0", 12)),
                        MalformedHeader,
                        [](const MalformedHeader& e)
                        {
                          return boost::regex_search(e.what(),
                                                     boost::regex("[Ii]ncorrect payload length"))
                            && boost::regex_search(e.what(), boost::regex("command \"filh\""))
                            && boost::regex_search(e.what(), boost::regex("\\b1\\b"))
                            && boost::regex_search(e.what(), boost::regex("\\b4\\b"));
                        });
  BOOST_CHECK_EXCEPTION(factory.create(std::string("logsclnh\2\0\0\0", 12)),
                        MalformedHeader,
                        [](const MalformedHeader& e)
                        {
                          return boost::regex_search(e.what(),
                                                     boost::regex("[Ii]ncorrect payload length"))
                            && boost::regex_search(e.what(), boost::regex("command \"clnh\""))
                            && boost::regex_search(e.what(), boost::regex("\\b2\\b"))
                            && boost::regex_search(e.what(), boost::regex("\\b4\\b"));
                        });
}

BOOST_FIXTURE_TEST_CASE(error_on_missing_logs_string_in_header, F)
{
  BOOST_CHECK_EXCEPTION(factory.create(std::string("asdfackn\0\0\0\0", 12)),
                        MalformedHeader,
                        [](const MalformedHeader& e)
                        {
                          return boost::regex_search(e.what(), boost::regex("[Mm]issing start sequence \"logs\"."));
                        });
}

BOOST_FIXTURE_TEST_CASE(logic_error_on_wrong_number_of_bytes_in_header, F)
{
  BOOST_CHECK_THROW(factory.create(std::string("logsackn\0\0\0", 11)),
                    std::logic_error);
  BOOST_CHECK_THROW(factory.create(std::string("logsackn\0\0\0\0\0", 13)),
                    std::logic_error);
  BOOST_CHECK_THROW(factory.create(std::string("ogsackn\0\0\0\0", 11)),
                    std::logic_error);
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
