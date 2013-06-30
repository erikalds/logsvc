/* Source file created: 2013-04-13

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

#include "logsvc_daemon/SessionInitiator.h"

#include "logsvc_daemon/SocketSessionFactory.h"
#include "logsvc_daemon/SocketSession.h"
#include "network/Socket.h"
#include "network/SocketAcceptor.h"
#include <algorithm>
#include <iostream>

namespace logsvc
{
  namespace daemon
  {

    SessionInitiator::SessionInitiator(SocketSessionFactory& factory,
                                       network::SocketAcceptor& acceptor) :
      acceptor(acceptor),
      factory(factory),
      live_sessions()
    {
      acceptor.async_accept(*this);
    }

    SessionInitiator::~SessionInitiator()
    {
      for (auto iter = std::begin(live_sessions); iter != std::end(live_sessions); ++iter)
        (*iter)->remove_socket_session_listener(this);
    }

    void SessionInitiator::accept_requested(std::unique_ptr<network::Socket> socket)
    {
      std::unique_ptr<SocketSession> session = factory.create_session(std::move(socket));
      //std::clog << "INFO [SessionInitiator]: accepted socket session " << session.get() << std::endl;
      session->start_listen();
      session->add_socket_session_listener(this);
      live_sessions.insert(std::move(session));
      acceptor.async_accept(*this);
    }

    void SessionInitiator::error_occurred(const std::string& message)
    {
      std::clog << "ERROR [SessionInitiator]: " << message << std::endl;
      acceptor.async_accept(*this);
    }

    void SessionInitiator::connection_lost(SocketSession* session)
    {
      //std::clog << "INFO [SessionInitiator]: connection lost " << session << std::endl;
      session->remove_socket_session_listener(this);
      auto iter = std::find_if(live_sessions.begin(), live_sessions.end(),
                               [&](const std::unique_ptr<SocketSession>& ss)
                               { return ss.get() == session; });
      if (iter != live_sessions.end())
        live_sessions.erase(iter);
    }

  } // namespace daemon
} // namespace logsvc
