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

#include "log/FileHandle.h"

#include "log/ClientExecutor.h"
#include "log/create_header.h"
#include "log/int_codec.h"

namespace logsvc
{
  namespace prot
  {

    FileHandle::FileHandle() :
      AbstractReceivable(4),
      fh(0)
    {
    }

    FileHandle::FileHandle(unsigned int handle) :
      AbstractReceivable(4),
      fh(handle)
    {
    }

    bool FileHandle::operator==(const FileHandle& other) const
    {
      return fh == other.fh;
    }

    bool FileHandle::operator<(const FileHandle& other) const
    {
      return fh < other.fh;
    }

    std::string FileHandle::get_header() const
    {
      return create_header("filh", 4);
    }

    std::string FileHandle::get_payload() const
    {
      return encode_32bit_int(fh);
    }

    void FileHandle::read_payload(const std::string& payload)
    {
      fh = decode_32bit_int(payload);;
    }

    std::unique_ptr<Deliverable> FileHandle::act(Executor& exec)
    {
      return std::unique_ptr<Deliverable>();
    }

    void FileHandle::act(ClientExecutor& exec)
    {
      exec.set_file_handle(*this);
    }

  } // namespace prot
} // namespace logsvc
