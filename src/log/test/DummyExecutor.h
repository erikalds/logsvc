#ifndef DUMMYEXECUTOR_H_
#define DUMMYEXECUTOR_H_

/* Header created: 2013-03-27

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
#include "log/ClientHandle.h"
#include "log/FileHandle.h"
#include <map>

namespace mock
{

  class DummyExecutor : public logsvc::prot::Executor
  {
  public:
    DummyExecutor() :
      fh_counter(0), open_file_fails(false), error_string("error"), messages(),
      write_message_fails(false), error_on_set_client_info(false)
    {}

    virtual logsvc::prot::FileHandle open_file(const boost::filesystem::path& filename)
    {
      if (open_file_fails)
        throw std::runtime_error(error_string);

      opened_file = filename;
      return logsvc::prot::FileHandle(++fh_counter);
    }

    virtual void write_message(const logsvc::prot::FileHandle& fh,
                               const std::string& message)
    {
      if (write_message_fails)
        throw std::runtime_error(error_string);
      messages[fh] += message;
    }

    virtual logsvc::prot::ClientHandle
    set_client_info(const std::string& name, const std::string& address)
    {
      if (error_on_set_client_info)
        throw std::runtime_error(error_string);

      client_name = name;
      client_address = address;
      return client_handle;
    }

    boost::filesystem::path opened_file;
    unsigned int fh_counter;
    bool open_file_fails;
    std::string error_string;

    std::map<logsvc::prot::FileHandle, std::string> messages;
    bool write_message_fails;

    std::string client_name;
    std::string client_address;
    logsvc::prot::ClientHandle client_handle;
    bool error_on_set_client_info;
  };

} // namespace mock

#endif // DUMMYEXECUTOR_H_
