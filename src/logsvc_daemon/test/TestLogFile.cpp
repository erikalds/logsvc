/* Source file created: 2013-01-27

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

#include "logsvc_daemon/LogFile.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#define BOOST_TEST_MODULE "logsvc daemon test suite"
#include <boost/test/unit_test.hpp>

#include <cstdlib>

BOOST_AUTO_TEST_SUITE(testLogFile)

struct F
{
  F() : tempfilename() {}
  ~F()
  {
    if (boost::filesystem::exists(tempfilename))
      unlink(tempfilename.c_str());
  }

  void create_tempfile()
  {
    tempfilename = "./TestLogFile_XXXXXX";
    int fd = mkstemp(&tempfilename[0]);
    close(fd);
  }

  /// Does not create tempfilename
  void generate_tempfilename()
  {
    tempfilename = "./TestLogFile_XXXXXX";
    tempfilename = mktemp(&tempfilename[0]);
  }

  boost::filesystem::path filename() const
  { return boost::filesystem::path(tempfilename); }

  logsvc::daemon::LogFile create_LogFile()
  {
    return logsvc::daemon::LogFile(filename());
  }

  std::string read_file_contents() const
  {
    boost::filesystem::ifstream in(filename());
    std::ostringstream ost;
    ost << in.rdbuf();
    return ost.str();
  }

  std::string tempfilename;
};

BOOST_FIXTURE_TEST_CASE(notExistingFileGivenToCtor_FileIsCreated, F)
{
  generate_tempfilename();
  create_LogFile();
  BOOST_REQUIRE(boost::filesystem::exists(filename()));
}

BOOST_FIXTURE_TEST_CASE(existingFileGivenToCtor_noProblem, F)
{
  create_tempfile();
  BOOST_CHECK_NO_THROW(create_LogFile());
}

BOOST_FIXTURE_TEST_CASE(writeString_StringIsWritten, F)
{
  create_tempfile();
  logsvc::daemon::LogFile lf = create_LogFile();
  lf.write("asdf");
  BOOST_CHECK_EQUAL("asdf", read_file_contents());
}

BOOST_FIXTURE_TEST_CASE(writeStrings_StringsAreAppended, F)
{
  create_tempfile();
  logsvc::daemon::LogFile lf = create_LogFile();
  lf.write("foo");
  lf.write("bar");
  BOOST_CHECK_EQUAL("foobar", read_file_contents());
}

BOOST_FIXTURE_TEST_CASE(writeStringsToNotExistingFile_CreatedFileIsUsed, F)
{
  generate_tempfilename();
  logsvc::daemon::LogFile lf = create_LogFile();
  lf.write("as");
  lf.write("df");
  BOOST_CHECK_EQUAL("asdf", read_file_contents());
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
