/* Source file created: 2013-05-09

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

#include "logsvccpp/client/ConnectedLogFile.h"

#include "logsvccpp/client/SessionConnection.h"
#include "log/CloseFile.h"
#include "log/File.h"
#include "log/FileHandle.h"

namespace logsvc
{
  namespace client
  {

    ConnectedLogFile::ConnectedLogFile(SessionConnection& connection,
                                       const boost::filesystem::path& path) :
      connection(connection)
    {
      std::unique_ptr<prot::Receivable> recv = connection.send(prot::File(path));
      recv->act(*this);
    }

    ConnectedLogFile::~ConnectedLogFile()
    {
      connection.send(prot::CloseFile(*file));
    }

    void ConnectedLogFile::set_file_handle(const prot::FileHandle& fh)
    {
      file.reset(new prot::FileHandle(fh));
    }

    void ConnectedLogFile::set_error(const std::string& error_string)
    {
    }

  } // namespace client
} // namespace logsvc
