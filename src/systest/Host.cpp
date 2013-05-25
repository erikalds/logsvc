/* Source file created: 2013-05-03

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
#include <csignal>
#include <iostream>

void abort_handler(int sig)
{
  std::cerr << "Abort called: " << sig << std::endl;
}

int main()
{
  std::signal(SIGABRT, &abort_handler);
  try
  {
    logsvc::Host host0("appname");
    logsvc::Host host1("appname", "127.0.0.1");
    return 0;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Host: exception caught: " << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "Host: unknown exception caught." << std::endl;
    return 2;
  }
}
