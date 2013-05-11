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
#include "logsvccpp/FileWriteError.h"
#include "logsvccpp/OpenFileError.h"
#include "log/CloseFile.h"
#include "log/File.h"
#include "log/FileHandle.h"
#include "log/Message.h"

namespace logsvc
{
  namespace client
  {

    ConnectedLogFile::ConnectedLogFile(SessionConnection& connection,
                                       const boost::filesystem::path& path) :
      connection(connection),
      file(),
      error_string(),
      file_path(path)
    {
      std::unique_ptr<prot::Receivable> recv = connection.send(prot::File(path));
      recv->act(*this);
      if (!error_string.empty())
        throw OpenFileError(path, error_string);
    }

    ConnectedLogFile::~ConnectedLogFile()
    {
      connection.send(prot::CloseFile(*file));
    }

    void ConnectedLogFile::writeln(const std::string& message)
    {
      std::unique_ptr<prot::Receivable> recv
        = connection.send(prot::Message(message, *file));
      recv->act(*this);

      if (!error_string.empty())
      {
        std::string current_error_string;
        std::swap(current_error_string, error_string);
        throw FileWriteError(file_path, current_error_string);
      }
    }

    void ConnectedLogFile::set_file_handle(const prot::FileHandle& fh)
    {
      file.reset(new prot::FileHandle(fh));
    }

    void ConnectedLogFile::set_error(const std::string& error)
    {
      error_string = error;
    }

    void ConnectedLogFile::success()
    {
    }

  } // namespace client
} // namespace logsvc
