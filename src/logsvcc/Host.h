#ifndef logsvcc_HOST_H_
#define logsvcc_HOST_H_

/* Header created: 2014-04-14

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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct logsvc_Host_t
{
} logsvc_Host;

logsvc_Host* logsvc_connect_host(const char* hostname, const char* appname);
void logsvc_disconnect(logsvc_Host* host);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // logsvcc_HOST_H_
