#ifndef DEFAULTSOCKETSESSION_H_
#define DEFAULTSOCKETSESSION_H_

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

#include "logsvc_daemon/SocketSession.h"
#include "network/SocketListener.h"
#include "network/SocketStateListener.h"
#include <memory>
#include <set>

namespace network
{
  class Socket;
}

namespace logsvc
{
  namespace prot
  {
    class Executor;
    class Receivable;
    class ReceivableFactory;
  } // namespace prot

  namespace daemon
  {

    class DefaultSocketSession : public SocketSession,
                                 public network::SocketListener,
                                 public network::SocketStateListener
    {
    public:
      DefaultSocketSession(std::unique_ptr<network::Socket> socket,
                           std::unique_ptr<prot::Executor> exec,
                           std::unique_ptr<prot::ReceivableFactory> rf);
      virtual ~DefaultSocketSession();

      virtual void start_listen();

      virtual void add_socket_session_listener(SocketSessionListener*);
      virtual void remove_socket_session_listener(SocketSessionListener*);

    private:
      virtual void bytes_received(const std::string& bytes);
      virtual void write_succeeded();
      virtual void error_occurred(const std::string& message);

      virtual void connection_lost(network::Socket* socket);

      void listen_for_new_header();

    private:
      std::unique_ptr<network::Socket> the_socket;
      std::unique_ptr<prot::ReceivableFactory> the_receivable_factory;
      std::unique_ptr<prot::Receivable> current_receivable;
      std::unique_ptr<prot::Executor> executor;
      std::set<SocketSessionListener*> listeners;
    };

  } // namespace daemon
} // namespace logsvc

#endif // DEFAULTSOCKETSESSION_H_
