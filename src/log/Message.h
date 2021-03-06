#ifndef MESSAGE_H_
#define MESSAGE_H_

/* Header created: 2013-03-27

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

#include "log/AbstractReceivable.h"
#include "log/Deliverable.h"
#include "log/FileHandle.h"

#include <memory>
#include <string>

namespace logsvc
{
  namespace prot
  {

    class Executor;

    class Message : public AbstractReceivable,
                    public Deliverable
    {
    public:
      Message(std::size_t payload_length);
      Message(const std::string& msg, const FileHandle& fh);

      std::string get_message() const;
      FileHandle get_filehandle() const;

      virtual void read_payload(const std::string& payload);
      virtual std::unique_ptr<Deliverable> act(Executor& exec);
      virtual void act(ClientExecutor& /*exec*/) {}

      virtual std::string get_header() const;
      virtual std::string get_payload() const;

    private:
      std::string message;
      FileHandle fh;
    };

  } //   namespace prot
} // namespace logsvc

#endif // MESSAGE_H_
