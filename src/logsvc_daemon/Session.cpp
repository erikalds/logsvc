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
#include "log/File.h"
#include "log/FileHandle.h"

namespace logsvc
{
  namespace daemon
  {

    Session::Session(FileFactory& ff) :
      file_factory(ff),
      file_handle_counter(1),
      open_files()
    {
    }

    prot::FileHandle Session::open_file(const prot::File& f)
    {
      auto iter = open_files.find(f.get_name());
      if (iter != open_files.end())
        return prot::FileHandle(iter->second);

      file_factory.open_file(f.get_name());
      unsigned int fh = file_handle_counter++;
      open_files.insert(std::make_pair(f.get_name(), fh));
      return prot::FileHandle(fh);
    }

  } // namespace daemon
} // namespace logsvc
