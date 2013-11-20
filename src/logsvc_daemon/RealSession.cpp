/* Source file created: 2013-03-03

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

#include "logsvc_daemon/RealSession.h"

#include "logsvc_daemon/ClientHandleFactory.h"
#include "logsvc_daemon/FileFactory.h"
#include "logsvc_daemon/File.h"
#include "logsvc_daemon/TimestampFactory.h"
#include "log/ClientHandle.h"
#include "log/File.h"
#include "log/FileHandle.h"
#include <egen/lookup.h>

namespace logsvc
{
  namespace daemon
  {

    using prot::FileHandle;
    using boost::filesystem::path;

    RealSession::RealSession(TimestampFactory& tsfac,
                             FileFactory& ff,
                             ClientHandleFactory& clhfac) :
      file_factory(ff),
      timestamp_factory(tsfac),
      file_handle_counter(1),
      open_filehandles(),
      open_files(),
      client_name(),
      client_address(),
      client_handle(new prot::ClientHandle(clhfac.generate_client_handle()))
    {
    }

    RealSession::~RealSession()
    {
    }

    FileHandle RealSession::open_file(const path& filename)
    {
      FileHandle fh = get_filehandle_for_path(filename);
      open_file_if_necessary(fh, filename);
      return fh;
    }

    FileHandle RealSession::get_filehandle_for_path(const path& filename)
    {
      auto iter = open_filehandles.find(filename);
      if (iter != open_filehandles.end())
        return iter->second;
      else
      {
        FileHandle fh = FileHandle(file_handle_counter++);
        open_filehandles.insert(std::make_pair(filename, fh));
        return fh;
      }
    }

    void RealSession::open_file_if_necessary(const FileHandle& fh, const path& filename)
    {
      auto iter2 = open_files.find(fh);
      if (iter2 == open_files.end())
      {
        std::shared_ptr<File> f = file_factory.open_file(filename);
        open_files.insert(std::make_pair(fh, f));
      }
    }

    void RealSession::close_file(const FileHandle& fh)
    {
      auto iter = open_files.find(fh);
      if (iter != open_files.end())
        open_files.erase(iter);
    }

    void RealSession::write_message(const FileHandle& fh, const std::string& message)
    {
      std::shared_ptr<File> f = egen::lookup(fh, open_files, std::shared_ptr<File>());
      if (!f)
        throw std::runtime_error("Invalid file handle.");

      std::ostringstream ost;
      ost << "[" << timestamp_factory.get_timestamp() << ":"
          << client_name << ":"
          << client_address << "] "
          << message << "\n";
      f->write(ost.str());
    }

    prot::ClientHandle RealSession::set_client_info(const std::string& name,
                                                    const std::string& address)
    {
      client_name = name;
      client_address = address;
      return *client_handle;
    }

    void RealSession::disconnect_client(const prot::ClientHandle& client)
    {
      for (auto f : open_files)
        write_message(f.first, "Disconnected from logsvc daemon.");
    }

  } // namespace daemon
} // namespace logsvc
