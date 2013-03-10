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

#include "logsvc_daemon/Session.h"

#include "logsvc_daemon/FileFactory.h"
#include "logsvc_daemon/File.h"
#include "log/Client.h"
#include "log/File.h"
#include "log/FileHandle.h"
#include <egen/lookup.h>

namespace logsvc
{
  namespace daemon
  {

    Session::Session(const prot::Client& c, FileFactory& ff) :
      client(new prot::Client(c)),
      file_factory(ff),
      file_handle_counter(1),
      open_filehandles(),
      open_files()
    {
    }

    Session::~Session()
    {
    }

    prot::FileHandle Session::open_file(const prot::File& prot_file)
    {
      auto iter = open_filehandles.find(prot_file.get_name());
      if (iter != open_filehandles.end())
        return prot::FileHandle(iter->second);

      std::shared_ptr<File> f = file_factory.open_file(prot_file.get_name());
      prot::FileHandle fh(file_handle_counter++);
      open_filehandles.insert(std::make_pair(prot_file.get_name(), fh));
      open_files.insert(std::make_pair(fh, f));

      return fh;
    }

    void Session::write_message(const prot::FileHandle& fh, const std::string& message)
    {
      std::shared_ptr<File> f = egen::lookup(fh, open_files, std::shared_ptr<File>());
      f->write("[" + client->get_name() + "]" + message);
    }

  } // namespace daemon
} // namespace logsvc
