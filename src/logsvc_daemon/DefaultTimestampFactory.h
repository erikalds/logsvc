#ifndef DEFAULTTIMESTAMPFACTORY_H_
#define DEFAULTTIMESTAMPFACTORY_H_

/* Header created: 2013-04-21

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

#include "logsvc_daemon/TimestampFactory.h"
#include <memory>

namespace boost { namespace posix_time { class ptime; } }

namespace logsvc
{
  namespace daemon
  {

    class TimeGenerator;

    class DefaultTimestampFactory : public TimestampFactory
    {
    public:
      DefaultTimestampFactory();
      explicit DefaultTimestampFactory(std::unique_ptr<TimeGenerator> generator);
      DefaultTimestampFactory(DefaultTimestampFactory&& factory);
      DefaultTimestampFactory& operator=(DefaultTimestampFactory&& factory);

      virtual std::string get_timestamp() const;

    private:
      std::unique_ptr<TimeGenerator> generator;
    };



    class TimeGenerator
    {
    public:
      virtual ~TimeGenerator() = 0;

      virtual boost::posix_time::ptime generate() const = 0;
    };

    class DefaultTimeGenerator : public TimeGenerator
    {
    public:
      virtual boost::posix_time::ptime generate() const;
    };

  } // namespace daemon
} // namespace logsvc

inline logsvc::daemon::TimeGenerator::~TimeGenerator() {}

#endif // DEFAULTTIMESTAMPFACTORY_H_
