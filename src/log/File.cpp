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

#include "log/File.h"

#include "log/create_header.h"
#include "log/Deliverable.h"
#include "log/Executor.h"
#include "log/FileHandle.h"
#include "log/NotAcknowledged.h"

namespace logsvc
{
  namespace prot
  {

    File::File(std::size_t payload_length) :
      AbstractReceivable(payload_length),
      filename("invalid path")
    {
    }

    File::File(const boost::filesystem::path& fname) :
      AbstractReceivable(fname.string().size()),
      filename(fname)
    {
    }

    boost::filesystem::path File::get_name() const
    {
      if (filename == "invalid path")
        throw std::logic_error("Cannot get file path from half created File object.");

      return filename;
    }

    void File::read_payload(const std::string& payload)
    {
      filename = boost::filesystem::path(payload);
    }

    std::unique_ptr<Deliverable> File::act(Executor& exec)
    {
      try
      {
        FileHandle fh = exec.open_file(get_name());
        return std::unique_ptr<Deliverable>(new FileHandle(fh));
      }
      catch (const std::exception& e)
      {
        return std::unique_ptr<Deliverable>(new NotAcknowledged(e.what()));
      }
    }

    std::string File::get_header() const
    {
      return create_header("open", filename.string().size());
    }

    std::string File::get_payload() const
    {
      return filename.string();
    }

  } // namespace prot
} // namespace logsvc
