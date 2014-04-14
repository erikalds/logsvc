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

#include "logsvcc/Log.h"

#include "logsvcc/Host.h"
#include "logsvccpp/Log.h"
#include <cassert>
#include <memory>

logsvc_Log* logsvc_open(const char* filename, logsvc_Host* host)
{
  std::unique_ptr<logsvc_Log> log(new logsvc_Log);
  try
  {
    assert(host && host->impl);
    log->impl = new logsvc::Log(filename,
                                *reinterpret_cast<logsvc::Host*>(host->impl));
    return log.release();
  }
  catch (...)
  {
    return nullptr;
  }
}

void logsvc_close(logsvc_Log* logfile)
{
  delete reinterpret_cast<logsvc::Log*>(logfile->impl);
  delete logfile;
}

void logsvc_logln(logsvc_Log* logfile, const char* line)
{
}
