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

namespace logsvc
{
  namespace prot
  {

    FileHandle::FileHandle(unsigned int handle) :
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

  } // namespace prot
} // namespace logsvc
