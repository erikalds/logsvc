#ifndef SESSIONINITIATOR_H_
#define SESSIONINITIATOR_H_

/* Header created: 2013-04-13

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

#include "logsvc_daemon/SocketSessionListener.h"
#include "network/SocketAcceptListener.h"
#include <set>

namespace network
{

  class SocketAcceptor;

} // namespace network

namespace logsvc
{
  namespace daemon
  {

    class SocketSession;
    class SocketSessionFactory;

    class SessionInitiator : public network::SocketAcceptListener,
                             public SocketSessionListener
    {
    public:
      SessionInitiator(SocketSessionFactory& factory,
                       network::SocketAcceptor& acceptor);
      ~SessionInitiator();

    private:
      virtual void accept_requested(std::unique_ptr<network::Socket> socket);
      virtual void error_occurred(const std::string& message);

      virtual void connection_lost(SocketSession* session);

      network::SocketAcceptor& acceptor;
      SocketSessionFactory& factory;
      std::set<std::unique_ptr<SocketSession> > live_sessions;
    };

  } // namespace daemon
} // namespace logsvc

#endif // SESSIONINITIATOR_H_
