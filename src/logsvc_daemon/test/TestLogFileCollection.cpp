/* Source file created: 2013-04-19

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

#include "logsvc_daemon/FileFactory.h"
#include "logsvc_daemon/LogFile.h"
#include "logsvc_daemon/LogFileCollection.h"
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <set>

using namespace logsvc::daemon;

BOOST_AUTO_TEST_SUITE(testLogFileCollection)

struct F
{
  F() : lfc(), created_files() {}
  ~F()
  {
    for (const boost::filesystem::path& filename : created_files)
      if (boost::filesystem::exists(filename))
        unlink(filename.c_str());
  }

  LogFileCollection lfc;
  std::set<boost::filesystem::path> created_files;

  std::shared_ptr<File> create_file(const boost::filesystem::path& filename)
  {
    std::shared_ptr<File> file = lfc.open_file(filename);
    created_files.insert(filename);
    return file;
  }
};

BOOST_FIXTURE_TEST_CASE(is_a_FileFactory, F)
{
  BOOST_CHECK(dynamic_cast<FileFactory*>(&lfc));
}

BOOST_FIXTURE_TEST_CASE(open_file_gives_valid_ptr, F)
{
  std::shared_ptr<File> file = create_file("unittest_some_file.txt");
  BOOST_CHECK(file != nullptr);
}

BOOST_FIXTURE_TEST_CASE(open_file_twice_gives_same_ptr, F)
{
  std::shared_ptr<File> file0 = create_file("unittest_another_file.txt");
  BOOST_CHECK(file0 != nullptr);
  std::shared_ptr<File> file1 = create_file("unittest_another_file.txt");
  BOOST_CHECK_EQUAL(file0, file1);
}

BOOST_FIXTURE_TEST_CASE(open_two_different_files_gives_different_ptr, F)
{
  std::shared_ptr<File> file0 = create_file("one_file.txt");
  BOOST_CHECK(file0 != nullptr);
  std::shared_ptr<File> file1 = create_file("another_file.txt");
  BOOST_CHECK_NE(file0, file1);
}

BOOST_FIXTURE_TEST_CASE(created_files_are_LogFile_instances, F)
{
  std::shared_ptr<File> file = create_file("a.txt");
  BOOST_CHECK(file != nullptr);
  std::shared_ptr<LogFile> logfile = std::dynamic_pointer_cast<LogFile>(file);
  // So that we don't have to test LogFile functionality
  BOOST_CHECK(logfile != nullptr);
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
