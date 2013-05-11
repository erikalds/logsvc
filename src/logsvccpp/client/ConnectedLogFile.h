#ifndef CONNECTEDLOGFILE_H_
#define CONNECTEDLOGFILE_H_

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

#include "logsvccpp/client/RemoteLogFile.h"
#include "log/ClientExecutor.h"
#include <boost/filesystem/path.hpp>

namespace logsvc
{
  namespace prot { class FileHandle; }
  namespace client
  {

    class SessionConnection;

    class ConnectedLogFile : public RemoteLogFile,
                             public prot::ClientExecutor
    {
    public:
      ConnectedLogFile(SessionConnection& connection,
                       const boost::filesystem::path& path);
      ~ConnectedLogFile();

      virtual void writeln(const std::string& message);

      virtual void set_file_handle(const prot::FileHandle& fh);
      virtual void set_error(const std::string& error_string);
      virtual void success();

    private:
      SessionConnection& connection;
      std::unique_ptr<prot::FileHandle> file;
      std::string error_string;
      boost::filesystem::path file_path;
    };

  } // namespace client
} // namespace logsvc

#endif // CONNECTEDLOGFILE_H_
