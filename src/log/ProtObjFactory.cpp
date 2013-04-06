/* Source file created: 2013-04-06

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

#include "log/ProtObjFactory.h"

#include "log/int_codec.h"
#include "log/Client.h"
#include "log/ClientHandle.h"
#include "log/File.h"
#include "log/FileHandle.h"
#include "log/Message.h"
#include "log/Receivable.h"

namespace logsvc
{
  namespace prot
  {

    std::unique_ptr<Receivable> ProtObjFactory::create(const std::string& header)
    {
      std::size_t payload_length = decode_32bit_int(header.substr(8));
      if (header.substr(4, 4) == "open")
        return std::unique_ptr<Receivable>(new File(payload_length));
      else if (header.substr(4, 4) == "mesg")
        return std::unique_ptr<Receivable>(new Message(payload_length));
      else if (header.substr(4, 4) == "filh")
      {
        assert(payload_length == 4);
        return std::unique_ptr<Receivable>(new FileHandle);
      }
      else if (header.substr(4, 4) == "clnh")
      {
        assert(payload_length == 4);
        return std::unique_ptr<Receivable>(new ClientHandle);
      }
      else
        return std::unique_ptr<Receivable>(new Client(payload_length));
    }

  } // namespace prot
} // namespace logsvc
