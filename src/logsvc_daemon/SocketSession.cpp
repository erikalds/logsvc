/* Source file created: 2013-03-25

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

#include "logsvc_daemon/SocketSession.h"

#include "log/Receivable.h"
#include "log/ReceivableFactory.h"
#include "network/Socket.h"

namespace logsvc
{
  namespace daemon
  {

    SocketSession::SocketSession(network::Socket& socket, Session& session,
                                 prot::ReceivableFactory& rf) :
      the_socket(socket),
      the_receivable_factory(rf),
      current_receivable(nullptr)
    {
    }

    void SocketSession::start_listen()
    {
      the_socket.async_read(*this);
    }

    void SocketSession::receive_bytes(const std::string& bytes)
    {
      if (!current_receivable)
        current_receivable = the_receivable_factory.create(bytes);
      else
        current_receivable->read_payload(bytes);

      the_socket.async_read(*this);
    }

  } // namespace daemon
} // namespace logsvc
