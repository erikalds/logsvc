#ifndef DUMMYCLIENTEXECUTOR_H_
#define DUMMYCLIENTEXECUTOR_H_

/* Header created: 2013-05-11

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

#include "log/ClientExecutor.h"
#include "log/FileHandle.h"

namespace logsvc
{
  namespace mock
  {

    class DummyClientExecutor : public logsvc::prot::ClientExecutor
    {
    public:
      DummyClientExecutor() : file_handle(), error_string("NOT SET") {}

      virtual void set_file_handle(const logsvc::prot::FileHandle& fh)
      { file_handle = fh; }

      virtual void set_error(const std::string& s)
      { error_string = s; }

      logsvc::prot::FileHandle file_handle;
      std::string error_string;
    };

  } // namespace mock
} // namespace logsvc

#endif // DUMMYCLIENTEXECUTOR_H_
