/* Source file created: 2013-04-28

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

#include "logsvc_daemon/DefaultSessionFactory.h"

#include "logsvc_daemon/DefaultClientHandleFactory.h"
#include "logsvc_daemon/DefaultSocketSession.h"
#include "logsvc_daemon/DefaultTimestampFactory.h"
#include "logsvc_daemon/LogFileCollection.h"
#include "logsvc_daemon/RealSession.h"
#include "logsvc_daemon/SocketSession.h"
#include "log/ProtObjFactory.h"
#include "network/Socket.h"

namespace logsvc
{
  namespace daemon
  {

    DefaultSessionFactory::DefaultSessionFactory() :
      timestamp_factory(new DefaultTimestampFactory),
      file_collection(new LogFileCollection),
      client_handle_factory(new DefaultClientHandleFactory)
    {
    }

    DefaultSessionFactory::~DefaultSessionFactory()
    {
    }

    std::unique_ptr<SocketSession>
    DefaultSessionFactory::create_session(std::unique_ptr<network::Socket> socket)
    {
      std::unique_ptr<prot::Executor> session(new RealSession(*timestamp_factory,
                                                              *file_collection,
                                                              *client_handle_factory));
      std::unique_ptr<prot::ReceivableFactory> rf(new prot::ProtObjFactory);
      return std::unique_ptr<SocketSession>(new DefaultSocketSession(std::move(socket),
                                                                     std::move(session),
                                                                     std::move(rf)));
    }

  } // namespace daemon
} // namespace logsvc
