#ifndef SOCKETSESSION_H_
#define SOCKETSESSION_H_

/* Header created: 2013-03-25

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

#include "network/SocketListener.h"
#include <memory>

namespace network
{
  class Socket;
}

namespace logsvc
{
  namespace prot
  {
    class Receivable;
    class ReceivableFactory;
  } // namespace prot

  namespace daemon
  {

    class Session;

    class SocketSession : public network::SocketListener
    {
    public:
      SocketSession(network::Socket& socket, Session& session,
                    prot::ReceivableFactory& rf);

      void start_listen();

    private:
      virtual void receive_bytes(const std::string& bytes);

    private:
      network::Socket& the_socket;
      prot::ReceivableFactory& the_receivable_factory;
      std::unique_ptr<prot::Receivable> current_receivable;
    };

  } // namespace daemon
} // namespace logsvc

#endif // SOCKETSESSION_H_
