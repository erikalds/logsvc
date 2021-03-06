/* Source file created: DATE

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

#include "logsvcc/Host.h"
#include "logsvcc/Log.h"
#include <stdio.h>

int main(int argc, char** argv)
{
  logsvc_Host* host;
  logsvc_Log* log;
  int i;

  if (argc < 3)
    return -1;

  host = logsvc_connect_host("localhost", "cloglinetofile");
  if (!host)
  {
    fprintf(stderr, "Error: could not connect to logsvc on localhost\n");
    return -2;
  }

  log = logsvc_open(argv[1], host);
  if (!log)
  {
    fprintf(stderr, "Error: could not open logfile %s on localhost\n", argv[1]);
    return -3;
  }

  for (i = 2; i < argc; ++i)
    logsvc_logln(log, argv[i]);

  logsvc_close(log);
  logsvc_disconnect(host);

  return 0;
}
