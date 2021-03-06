#ifndef SOCKET_H_
#define SOCKET_H_

/* Header created: 2013-03-25

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

#include <string>

namespace network
{

  class SocketListener;
  class SocketStateListener;

  class Socket
  {
  public:
    virtual ~Socket() = 0;

    virtual void async_read(SocketListener& listener, std::size_t read_bytes) = 0;
    virtual void async_write(SocketListener& listener, const std::string& data) = 0;

    virtual void add_socket_state_listener(SocketStateListener* listener) = 0;
    virtual void remove_socket_state_listener(SocketStateListener* listener) = 0;

    virtual void keep_alive() = 0;
  };

  inline Socket::~Socket() {}

} // namespace network

#endif // SOCKET_H_
