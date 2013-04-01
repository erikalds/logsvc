/* Source file created: 2013-03-27

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

#include "log/Message.h"

#include "log/create_header.h"
#include "log/Executor.h"
#include "log/Acknowledged.h"
#include "log/NotAcknowledged.h"
#include <cassert>

namespace logsvc
{
  namespace prot
  {

    Message::Message(std::size_t payload_length) :
      AbstractReceivable(payload_length),
      message("a message"),
      fh(0x3456)
    {
    }

    Message::Message(const std::string& msg, const FileHandle& fh) :
      AbstractReceivable(4 + msg.size()),
      message(msg),
      fh(fh)
    {
    }

    std::string Message::get_message() const
    {
      return message;
    }

    FileHandle Message::get_filehandle() const
    {
      return fh;
    }

    void Message::read_payload(const std::string& payload)
    {
      assert(payload.size() >= 4);
      unsigned int handle(0);
      handle |= payload[0];
      handle |= payload[1] << 8;
      handle |= payload[2] << 16;
      handle |= payload[3] << 24;
      fh = FileHandle(handle);
      message = payload.substr(4);
    }

    std::unique_ptr<Deliverable> Message::act(Executor& exec)
    {
      try
      {
        exec.write_message(fh, message);
        return std::unique_ptr<Deliverable>(new Acknowledged);
      }
      catch (const std::exception& e)
      {
        return std::unique_ptr<Deliverable>(new NotAcknowledged(e.what()));
      }
    }

    std::string Message::get_header() const
    {
      return create_header("mesg", 4 + message.size());
    }

    std::string Message::get_payload() const
    {
      return fh.get_payload() + message;
    }

  } // namespace prot
} // namespace logsvc
