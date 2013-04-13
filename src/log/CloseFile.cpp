/* Source file created: 2013-04-07

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

#include "log/CloseFile.h"

#include "log/create_header.h"
#include "log/Acknowledged.h"
#include "log/Executor.h"
#include "log/NotAcknowledged.h"

namespace logsvc
{
  namespace prot
  {

    CloseFile::CloseFile() :
      AbstractReceivable(FileHandle().get_payload_length()),
      file_handle()
    {
    }

    CloseFile::CloseFile(const FileHandle& fh) :
      AbstractReceivable(fh.get_payload_length()),
      file_handle(fh)
    {
    }

    std::string CloseFile::get_header() const
    {
      return create_header("clos", 4);
    }

    std::string CloseFile::get_payload() const
    {
      return file_handle.get_payload();
    }

    void CloseFile::read_payload(const std::string& payload)
    {
      file_handle.read_payload(payload);
    }

    std::unique_ptr<Deliverable> CloseFile::act(Executor& exec)
    {
      try
      {
        exec.close_file(file_handle);
        return std::unique_ptr<Deliverable>(new Acknowledged);
      }
      catch (const std::exception& e)
      {
        return std::unique_ptr<Deliverable>(new NotAcknowledged(e.what()));
      }
    }

  } // namespace prot
} // namespace logsvc
