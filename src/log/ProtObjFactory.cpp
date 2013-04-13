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
#include "log/Acknowledged.h"
#include "log/Client.h"
#include "log/ClientHandle.h"
#include "log/CloseFile.h"
#include "log/File.h"
#include "log/FileHandle.h"
#include "log/MalformedHeader.h"
#include "log/Message.h"
#include "log/NotAcknowledged.h"
#include "log/Receivable.h"
#include "log/UnknownProtocolObjectType.h"
#include <egen/lookup.h>

namespace logsvc
{
  namespace prot
  {
    namespace
    {

      void check_payload_length(std::size_t actual, std::size_t expected, const std::string& cmd)
      {
        if (actual == expected)
          return;

        std::ostringstream ost;
        ost << "Malformed header format: Incorrect payload length for command \""
            << cmd << "\".  Expected " << expected << ", was " << actual << ".";
        throw MalformedHeader(ost.str());
      }

    } // anonymous namespace

    ProtObjFactory::ProtObjFactory()
    {
      creators["open"] = [](std::size_t pl) { return new File(pl); };
      creators["mesg"] = [](std::size_t pl) { return new Message(pl); };
      creators["filh"] = [](std::size_t pl) { check_payload_length(pl, 4, "filh");
                                              return new FileHandle; };
      creators["clos"] = [](std::size_t pl) { check_payload_length(pl, 4, "clos");
                                              return new CloseFile; };
      creators["clnh"] = [](std::size_t pl) { check_payload_length(pl, 4, "clnh");
                                              return new ClientHandle; };
      creators["ackn"] = [](std::size_t pl) { check_payload_length(pl, 0, "ackn");
                                              return new Acknowledged; };
      creators["nack"] = [](std::size_t pl) { return new NotAcknowledged(pl); };
      creators["clnt"] = [](std::size_t pl) { return new Client(pl); };
    }

    std::unique_ptr<Receivable> ProtObjFactory::create(const std::string& header)
    {
      if (header.size() != 12)
        throw std::logic_error("Wrong number of bytes in header.");

      const std::string start_seq = header.substr(0, 4);
      if (start_seq != "logs")
        throw MalformedHeader("Malformed header format: Missing start sequence \"logs\".");

      const std::string typespec = header.substr(4, 4);
      auto create_fun = egen::lookup(typespec, creators,
                                     std::function<Receivable*(std::size_t)>());
      if (!create_fun)
        throw UnknownProtocolObjectType(typespec);

      const std::size_t payload_length = decode_32bit_int(header.substr(8));
      return std::unique_ptr<Receivable>(create_fun(payload_length));
    }

  } // namespace prot
} // namespace logsvc
