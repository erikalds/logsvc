#ifndef REALSESSION_H_
#define REALSESSION_H_

/* Header created: 2013-03-03

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

#include "log/Executor.h"
#include <boost/filesystem/path.hpp>
#include <map>

namespace logsvc
{
  namespace prot
  {

    class ClientHandle;
    class FileHandle;

  } // namespace prot

  namespace daemon
  {
    class File;
    class FileFactory;
    class TimestampFactory;

    class RealSession : public prot::Executor
    {
    public:
      RealSession(TimestampFactory& tsfac, FileFactory& ff);
      ~RealSession();

      virtual prot::FileHandle open_file(const boost::filesystem::path& filename);
      virtual void close_file(const prot::FileHandle& fh);
      virtual void write_message(const prot::FileHandle& fh,
                                 const std::string& message);
      virtual prot::ClientHandle set_client_info(const std::string& name,
                                                 const std::string& address);

    private:
      prot::FileHandle get_filehandle_for_path(const boost::filesystem::path& filename);
      void open_file_if_necessary(const prot::FileHandle& fh,
                                  const boost::filesystem::path& filename);

      FileFactory& file_factory;
      TimestampFactory& timestamp_factory;
      unsigned int file_handle_counter;
      std::map<boost::filesystem::path, prot::FileHandle> open_filehandles;
      std::map<prot::FileHandle, std::shared_ptr<File> > open_files;
      std::string client_name, client_address;
    };

  } // namespace daemon
} // namespace logsvc

#endif // REALSESSION_H_
