/* Source file created: 2014-04-14

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

#include "logsvccpp/Host.h"
#include "logsvccpp/Log.h"
#include <boost/python.hpp>

using namespace boost::python;

namespace {

  class PyLog
  {
  public:
    PyLog(const std::string& logfile, logsvc::Host& host) :
      log(new logsvc::Log(logfile, host)) {}

    void logln(const std::string& line) { log->logln(line); }
    void close() { log.reset(); }

  private:
    std::unique_ptr<logsvc::Log> log;
  };

}


BOOST_PYTHON_MODULE(logsvcpy)
{
  class_<logsvc::Host>("Host",
                       init<std::string, std::string>());

  class_<PyLog, boost::noncopyable>("Log",
                                    init<std::string, logsvc::Host&>())
    .def("logln", &PyLog::logln)
    .def("close", &PyLog::close);
}
