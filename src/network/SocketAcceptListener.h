#ifndef SOCKETACCEPTLISTENER_H_
#define SOCKETACCEPTLISTENER_H_

/* Header created: 2013-04-13

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

#include <memory>

namespace network
{

  class Socket;

  class SocketAcceptListener
  {
  public:
    virtual ~SocketAcceptListener() = 0;

    virtual void accept_requested(std::unique_ptr<Socket> socket) = 0;
    virtual void error_occurred(const std::string& message) = 0;
  };

} // namespace network

inline network::SocketAcceptListener::~SocketAcceptListener() {}

#endif // SOCKETACCEPTLISTENER_H_
