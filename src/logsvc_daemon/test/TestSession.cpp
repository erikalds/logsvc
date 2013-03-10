/* Source file created: 2013-03-03

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

#include "logsvc_daemon/FileFactory.h"
#include "logsvc_daemon/Session.h"
#include "logsvc_daemon/File.h"
#include "log/Client.h"
#include "log/File.h"
#include "log/FileHandle.h"
#include <egen/lookup.h>
#include <boost/filesystem/path.hpp>
#include <boost/test/unit_test.hpp>
#include <set>

using namespace logsvc::daemon;

BOOST_AUTO_TEST_SUITE(testService)

class DummyFile : public logsvc::daemon::File
{
public:
  virtual void write(const std::string& message)
  { contents += message; }

  std::string contents;
};

class DummyFileFactory : public FileFactory
{
public:
  DummyFileFactory() : opened_files() {}

  virtual std::shared_ptr<File> open_file(const boost::filesystem::path& p)
  {
    auto inserted = opened_files.insert(std::make_pair(p, std::make_shared<DummyFile>()));
    return inserted.first->second;
  }

  bool has_opened(const boost::filesystem::path& p)
  { return opened_files.count(p); }

  DummyFile* get_file(const boost::filesystem::path& p) const
  { return egen::lookup(p, opened_files, std::shared_ptr<DummyFile>()).get(); }

private:
  std::map<boost::filesystem::path, std::shared_ptr<DummyFile> > opened_files;
};

struct F
{
  F() : ff(), client("client"), session(client, ff),
        bracket_open_pos(std::string::npos), clientpos(std::string::npos),
        bracket_close_pos(std::string::npos), msgpos(std::string::npos) {}
  ~F() {}

  DummyFileFactory ff;
  logsvc::prot::Client client;
  Session session;
  std::size_t bracket_open_pos;
  std::size_t clientpos;
  std::size_t ip_pos;
  std::size_t bracket_close_pos;
  std::size_t msgpos;
  std::string contents;

  logsvc::prot::FileHandle open_file(const std::string& filename)
  {
    logsvc::prot::File f(filename);
    return session.open_file(f);
  }

  void write_message_and_find_line_positions()
  {
    logsvc::prot::FileHandle fh = open_file("asdf.txt");
    session.write_message(fh, "hallo");
    DummyFile* dummy = ff.get_file("asdf.txt");
    BOOST_REQUIRE(dummy != nullptr);
    contents = dummy->contents;
    bracket_open_pos = contents.find("[");
    clientpos = contents.find("client");

    std::ostringstream ost;
    ost << client.get_ip_address();
    ip_pos = contents.find(ost.str());

    bracket_close_pos = contents.find("]");
    msgpos = contents.find("hallo");
  }
};

BOOST_FIXTURE_TEST_CASE(openFile_OpensCorrectFile, F)
{
  open_file("asdf.txt");
  BOOST_CHECK(ff.has_opened("asdf.txt"));
}

BOOST_FIXTURE_TEST_CASE(openFile_returnsFileHandle, F)
{
  logsvc::prot::FileHandle fh0 = open_file("asdf.txt");
  // let's define FileHandle(0) as an invalid FileHandle, don't know
  // yet if that will be useful
  BOOST_CHECK(fh0 != logsvc::prot::FileHandle(0));
  logsvc::prot::FileHandle fh1 = open_file("foobar.txt");
  BOOST_CHECK(fh1 != logsvc::prot::FileHandle(0));
  BOOST_CHECK(fh0 != fh1);
}

BOOST_FIXTURE_TEST_CASE(openSameFileTwice_sameFileHandle, F)
{
  BOOST_CHECK(open_file("foo.txt") == open_file("foo.txt"));
}

BOOST_FIXTURE_TEST_CASE(writeToOpenedFile_stringIsAdded, F)
{
  write_message_and_find_line_positions();
  BOOST_CHECK(msgpos != std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(writeToOpenedFile_clientIsAddedBeforeMessage, F)
{
  write_message_and_find_line_positions();
  BOOST_CHECK(clientpos != std::string::npos);
  BOOST_CHECK_LT(clientpos, msgpos);
}

BOOST_FIXTURE_TEST_CASE(writeToOpenedFile_bracketsAreAddedAroundClient, F)
{
  write_message_and_find_line_positions();

  BOOST_CHECK(bracket_open_pos != std::string::npos);
  BOOST_CHECK(bracket_close_pos != std::string::npos);

  BOOST_CHECK_LT(bracket_open_pos, clientpos);
  BOOST_CHECK_LT(clientpos, bracket_close_pos);
  BOOST_CHECK_LT(bracket_close_pos, msgpos);
}

BOOST_FIXTURE_TEST_CASE(writeToOpenedFile_whitespaceAfterBracket, F)
{
  write_message_and_find_line_positions();

  BOOST_CHECK_LT(bracket_close_pos + 1, msgpos);
  BOOST_CHECK_EQUAL(' ', contents[bracket_close_pos + 1]);
}

BOOST_FIXTURE_TEST_CASE(writeToOpenedFile_newLineAtEndOfFile, F)
{
  write_message_and_find_line_positions();
  BOOST_CHECK_EQUAL('\n', contents.back());
}

BOOST_FIXTURE_TEST_CASE(writeToOpenedFile_clientIPIsAddedAfterClient, F)
{
  write_message_and_find_line_positions();
  BOOST_CHECK_NE(ip_pos, std::string::npos);
  BOOST_CHECK_LT(clientpos, ip_pos);
}

BOOST_FIXTURE_TEST_CASE(writeToOpenedFile_clientIPIsWithinBrackets, F)
{
  write_message_and_find_line_positions();
  BOOST_CHECK_LT(bracket_open_pos, ip_pos);
  BOOST_CHECK_LT(ip_pos, bracket_close_pos);
}

BOOST_FIXTURE_TEST_CASE(writeToOpenedFile_clientIPIsAfterColon, F)
{
  write_message_and_find_line_positions();
  BOOST_CHECK_EQUAL(':', contents[ip_pos - 1]);
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
