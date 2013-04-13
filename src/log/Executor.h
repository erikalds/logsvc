#ifndef EXECUTOR_H_
#define EXECUTOR_H_

/* Header created: 2013-03-26

  logsvc - logging as a service
  Copyright (C) 2013 Erik �ldstedt Sund

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
    Erik �ldstedt Sund
    Darres veg 14
    NO-7540 KL�BU
    NORWAY
*/

#include <boost/filesystem/path.hpp>
#include <string>

namespace logsvc
{
  namespace prot
  {

    class ClientHandle;
    class FileHandle;

    class Executor
    {
    public:
      virtual ~Executor() = 0;

      virtual FileHandle open_file(const boost::filesystem::path& filename) = 0;
      virtual void close_file(const FileHandle& fh) = 0;
      virtual void write_message(const FileHandle& fh,
                                 const std::string& message) = 0;
      virtual ClientHandle set_client_info(const std::string& name,
                                           const std::string& address) = 0;
    };

    inline Executor::~Executor() {}

  } // namespace prot
} // namespace logsvc

#endif // EXECUTOR_H_
