/* Source file created: 2014-04-14

  logsvc - logging as a service
  Copyright (C) 2013 Erik �ldstedt Sund

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
    Erik �ldstedt Sund
    Darres veg 14
    NO-7540 KL�BU
    NORWAY
*/

#include "logsvcc/Host.h"
#include "logsvccpp/Host.h"
#include <memory>

logsvc_Host* logsvc_connect_host(const char* hostname, const char* appname)
{
  std::unique_ptr<logsvc_Host> host(new logsvc_Host);
  try
  {
    host->impl = new logsvc::Host(appname, hostname);
    return host.release();
  }
  catch (...)
  {
    return nullptr;
  }
}

void logsvc_disconnect(logsvc_Host* host)
{
  delete reinterpret_cast<logsvc::Host*>(host->impl);
  delete host;
}
