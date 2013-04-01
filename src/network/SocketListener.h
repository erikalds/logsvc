#ifndef SOCKETLISTENER_H_
#define SOCKETLISTENER_H_

/* Header created: 2013-04-01

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

#include <string>

namespace network
{

  class SocketListener
  {
  public:
    virtual ~SocketListener() = 0;

    virtual void receive_bytes(const std::string& bytes) = 0;
  };

} // namespace network

inline network::SocketListener::~SocketListener() {}

#endif // SOCKETLISTENER_H_
