/* Source file created: 2013-04-21

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
#include "logsvc_daemon/DefaultTimestampFactory.h"
#include "logsvc_daemon/TimestampFactory.h"
#include <boost/date_time/gregorian/gregorian_io.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

using logsvc::daemon::DefaultTimestampFactory;
using logsvc::daemon::DefaultTimeGenerator;
using logsvc::daemon::TimestampFactory;
using logsvc::daemon::TimeGenerator;

class MockTimeGenerator : public TimeGenerator
{
public:
  MockTimeGenerator(int year, int month, int day, int hour, int minute,
                    int second, int millisecond) :
    year(year),
    month(month),
    day(day),
    hour(hour),
    minute(minute),
    second(second),
    millisecond(millisecond)
  {}

  boost::posix_time::ptime generate() const
  { return boost::posix_time::ptime(boost::gregorian::date(year, month, day),
                                    boost::posix_time::time_duration(hour,
                                                                     minute,
                                                                     second,
                                                                     frac_second())); }

  int frac_second() const
  {
    return millisecond
      * (std::pow(10,
                  boost::posix_time::time_duration::resolution() + 1)
         / 1000);
  }

  int year, month, day, hour, minute, second, millisecond;
};

BOOST_AUTO_TEST_SUITE(testDefaultTimestampFactory)

struct F
{
  F() : factory(), mo(),
        re("([0-9]{4,4})\\.([0-9]{2,2})\\.([0-9]{2,2}) ([0-9]{2,2}):([0-9]{2,2}):([0-9]{2,2})\\.([0-9]{3,3})"),
        ts() {}
  ~F() {}

  DefaultTimestampFactory factory;
  boost::smatch mo;
  boost::regex re;
  std::string ts;

  void read_and_parse_timestamp()
  {
    ts = factory.get_timestamp();
    BOOST_CHECK_MESSAGE(boost::regex_match(ts, mo, re), ts);
  }

  void mock_time(int year, int month, int day, int hour, int minute,
                 int second, int millisecond)
  {
    std::unique_ptr<TimeGenerator> tgen(new MockTimeGenerator(year, month, day,
                                                              hour, minute,
                                                              second,
                                                              millisecond));
    factory = DefaultTimestampFactory(std::move(tgen));
    read_and_parse_timestamp();
  }

  int year() const { return boost::lexical_cast<int>(mo[1]); }
  int month() const { return boost::lexical_cast<int>(mo[2]); }
  int day() const { return boost::lexical_cast<int>(mo[3]); }
  int hour() const { return boost::lexical_cast<int>(mo[4]); }
  int minute() const { return boost::lexical_cast<int>(mo[5]); }
  int second() const { return boost::lexical_cast<int>(mo[6]); }
  int millisecond() const { return boost::lexical_cast<int>(mo[7]); }
};

BOOST_FIXTURE_TEST_CASE(is_a_TimestampFactory, F)
{
  BOOST_CHECK(dynamic_cast<TimestampFactory*>(&factory) != nullptr);
}

BOOST_FIXTURE_TEST_CASE(timestamp_has_correct_format, F)
{
  read_and_parse_timestamp();
}

BOOST_FIXTURE_TEST_CASE(timestamp_is_from_this_day_or_before, F)
{
  read_and_parse_timestamp();
  boost::posix_time::ptime datetime = boost::posix_time::microsec_clock::local_time();
  boost::gregorian::date today = datetime.date();
  boost::gregorian::date actual(year(), month(), day());
  BOOST_CHECK_GE(today, actual);
}

BOOST_FIXTURE_TEST_CASE(timestamp_is_from_this_millisecond_or_before, F)
{
  read_and_parse_timestamp();
  boost::posix_time::ptime datetime = boost::posix_time::microsec_clock::local_time();
  boost::posix_time::time_duration time_of_day = datetime.time_of_day();
  boost::posix_time::time_duration actual(hour(), minute(), second(), millisecond());
  BOOST_CHECK_GE(time_of_day, actual);
}

BOOST_FIXTURE_TEST_CASE(is_correct_year, F)
{
  mock_time(2013, 4, 27, 11, 46, 12, 132);
  BOOST_CHECK_EQUAL(2013, year());
  mock_time(2010, 12, 13, 14, 15, 16, 17);
  BOOST_CHECK_EQUAL(2010, year());
}

BOOST_FIXTURE_TEST_CASE(is_correct_month, F)
{
  mock_time(2013, 4, 27, 11, 46, 12, 132);
  BOOST_CHECK_EQUAL(4, month());
  mock_time(2010, 12, 13, 14, 15, 16, 17);
  BOOST_CHECK_EQUAL(12, month());
}

BOOST_FIXTURE_TEST_CASE(is_correct_day, F)
{
  mock_time(2013, 4, 27, 11, 46, 12, 132);
  BOOST_CHECK_EQUAL(27, day());
  mock_time(2010, 12, 1, 14, 15, 16, 17);
  BOOST_CHECK_EQUAL(1, day());
}

BOOST_FIXTURE_TEST_CASE(is_correct_hour, F)
{
  mock_time(2013, 4, 27, 11, 46, 12, 132);
  BOOST_CHECK_EQUAL(11, hour());
  mock_time(2010, 12, 13, 2, 15, 16, 17);
  BOOST_CHECK_EQUAL(2, hour());
}

BOOST_FIXTURE_TEST_CASE(is_correct_minute, F)
{
  mock_time(2013, 4, 27, 11, 46, 12, 132);
  BOOST_CHECK_EQUAL(46, minute());
  mock_time(2010, 12, 13, 2, 3, 16, 17);
  BOOST_CHECK_EQUAL(3, minute());
}

BOOST_FIXTURE_TEST_CASE(is_correct_second, F)
{
  mock_time(2013, 4, 27, 11, 46, 12, 132);
  BOOST_CHECK_EQUAL(12, second());
  mock_time(2010, 12, 13, 2, 3, 4, 17);
  BOOST_CHECK_EQUAL(4, second());
}

BOOST_FIXTURE_TEST_CASE(is_correct_millisecond, F)
{
  mock_time(2013, 4, 27, 11, 46, 12, 132);
  BOOST_CHECK_EQUAL(132, millisecond());
  mock_time(2010, 12, 13, 2, 3, 4, 17);
  BOOST_CHECK_EQUAL(17, millisecond());
  mock_time(2010, 12, 13, 2, 3, 4, 5);
  BOOST_CHECK_EQUAL(5, millisecond());
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
