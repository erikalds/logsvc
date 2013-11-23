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
#include "log/ClientHandle.h"
#include "log/Executor.h"
#include <boost/filesystem/path.hpp>
#include <map>
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
      explicit DummySessionConnection(const DSCKilledListener* listener = nullptr);
      ~DummySessionConnection();

      virtual std::future<std::unique_ptr<logsvc::prot::Receivable>>
      send(const logsvc::prot::Deliverable& deliverable);

      virtual logsvc::prot::FileHandle
      open_file(const boost::filesystem::path& filename);

      virtual void close_file(const logsvc::prot::FileHandle& fh);

      virtual void write_message(const logsvc::prot::FileHandle& fh,
                                 const std::string& message);

      virtual logsvc::prot::ClientHandle
      set_client_info(const std::string& name, const std::string& address);

      void disconnect_client(const logsvc::prot::ClientHandle& client) override;

      std::string client_name;
      std::string client_address;
      std::vector<boost::filesystem::path> opened_files;
      std::string open_file_error;
      std::map<boost::filesystem::path, std::string> file_contents;
      std::string write_message_error;
      logsvc::prot::ClientHandle client_handle;
      bool disconnected;
      std::string set_client_info_error;

    private:
      std::size_t lookup_idx(const logsvc::prot::FileHandle fh) const;

      std::map<logsvc::prot::FileHandle, std::size_t> fh_to_idx;
      const DSCKilledListener* listener;
    };

  } // namespace mock
} // namespace logsvc

#endif // DUMMYSESSIONCONNECTION_H_
