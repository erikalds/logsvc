#ifndef DUMMYSESSIONCONNECTION_H_
#define DUMMYSESSIONCONNECTION_H_

/* Header created: 2013-05-09

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

#include "logsvccpp/client/SessionConnection.h"
#include "log/Executor.h"
#include <boost/filesystem/path.hpp>
#include <vector>

namespace logsvc
{
  namespace mock
  {

    class DummySessionConnection;

    class DSCKilledListener
    {
    public:
      virtual ~DSCKilledListener() {}

      virtual void connection_killed(DummySessionConnection*) const = 0;
    };

    class DummySessionConnection : public logsvc::client::SessionConnection,
                                   public logsvc::prot::Executor
    {
    public:
      explicit DummySessionConnection(const DSCKilledListener* listener = nullptr) :
        client_name("UNSET"),
        client_address("UNSET"),
        listener(listener)
      {}

      ~DummySessionConnection()
      { if (listener) listener->connection_killed(this); }

      virtual void send(const logsvc::prot::Deliverable& deliverable);

      virtual logsvc::prot::FileHandle
      open_file(const boost::filesystem::path& filename);

      virtual void close_file(const logsvc::prot::FileHandle& /*fh*/) {}

      virtual void write_message(const logsvc::prot::FileHandle& /*fh*/,
                                 const std::string& /*message*/)
      {}

      virtual logsvc::prot::ClientHandle
      set_client_info(const std::string& name, const std::string& address);

      std::string client_name;
      std::string client_address;
      std::vector<boost::filesystem::path> opened_files;

    private:
      const DSCKilledListener* listener;
    };

  } // namespace mock
} // namespace logsvc

#endif // DUMMYSESSIONCONNECTION_H_

