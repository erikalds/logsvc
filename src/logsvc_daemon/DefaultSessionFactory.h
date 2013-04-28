#ifndef DEFAULTSESSIONFACTORY_H_
#define DEFAULTSESSIONFACTORY_H_

/* Header created: 2013-04-28

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

#include "logsvc_daemon/SocketSessionFactory.h"

namespace logsvc
{
  namespace daemon
  {

    class ClientHandleFactory;
    class FileFactory;
    class TimestampFactory;

    class DefaultSessionFactory : public SocketSessionFactory
    {
    public:
      DefaultSessionFactory();
      ~DefaultSessionFactory();

      virtual std::unique_ptr<SocketSession>
      create_session(std::unique_ptr<network::Socket> socket);

    private:
      std::unique_ptr<TimestampFactory> timestamp_factory;
      std::unique_ptr<FileFactory> file_collection;
      std::unique_ptr<ClientHandleFactory> client_handle_factory;
    };

  } // namespace daemon
} // namespace logsvc

#endif // DEFAULTSESSIONFACTORY_H_
