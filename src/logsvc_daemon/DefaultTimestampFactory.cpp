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

#include "logsvc_daemon/DefaultTimestampFactory.h"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace logsvc
{
  namespace daemon
  {
    DefaultTimestampFactory::DefaultTimestampFactory() :
      generator(std::unique_ptr<TimeGenerator>(new DefaultTimeGenerator))
    {
    }

    DefaultTimestampFactory::DefaultTimestampFactory(std::unique_ptr<TimeGenerator> generator) :
      generator(std::move(generator))
    {
    }

    DefaultTimestampFactory::DefaultTimestampFactory(DefaultTimestampFactory&& factory) :
      generator(std::move(factory.generator))
    {
    }

    DefaultTimestampFactory& DefaultTimestampFactory::operator=(DefaultTimestampFactory&& factory)
    {
      if (this == &factory)
        return *this;

      generator = std::move(factory.generator);
      return *this;
    }

    namespace
    {

      template<typename T>
      std::ostream& field(std::ostream& out, const T& val, std::size_t field_width)
      {
        out.fill('0');
        out.width(field_width);
        return out << std::right << val;
      }

      int to_ms(boost::posix_time::time_duration::fractional_seconds_type frac_sec)
      {
        return std::round((frac_sec * 1000 / (std::pow(10.0, boost::posix_time::time_duration::resolution() + 1))));
      }

    } // anonymous namespace

    std::string DefaultTimestampFactory::get_timestamp() const
    {
      std::ostringstream ost;
      boost::posix_time::ptime now = generator->generate();
      boost::gregorian::date today = now.date();
      field(ost, today.year(), 4) << ".";
      field(ost, today.month(), 2) << ".";
      field(ost, today.day(), 2) << " ";
      boost::posix_time::time_duration time_of_day = now.time_of_day();
      field(ost, time_of_day.hours(), 2) << ":";
      field(ost, time_of_day.minutes(), 2) << ":";
      field(ost, time_of_day.seconds(), 2) << ".";
      field(ost, to_ms(time_of_day.fractional_seconds()), 3);
      return ost.str();
    }



    boost::posix_time::ptime DefaultTimeGenerator::generate() const
    {
      return boost::posix_time::microsec_clock::local_time();
    }

  }
} // namespace logsvc
