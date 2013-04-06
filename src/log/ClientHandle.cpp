/* Source file created: 2013-04-01

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

#include "log/ClientHandle.h"

#include "log/create_header.h"
#include "log/int_codec.h"

namespace logsvc
{
  namespace prot
  {

    ClientHandle::ClientHandle() :
      AbstractReceivable(4),
      ch(0)
    {
    }

    ClientHandle::ClientHandle(unsigned int ch) :
      AbstractReceivable(4),
      ch(ch)
    {
    }

    std::string ClientHandle::get_header() const
    {
      return create_header("clnh", 4);
    }

    std::string ClientHandle::get_payload() const
    {
      return encode_32bit_int(ch);
    }

    bool ClientHandle::operator==(const ClientHandle& other) const
    {
      return ch == other.ch;
    }

    bool ClientHandle::operator<(const ClientHandle& other) const
    {
      return ch < other.ch;
    }

    void ClientHandle::read_payload(const std::string& payload)
    {
      ch = decode_32bit_int(payload);
    }

    std::unique_ptr<Deliverable> ClientHandle::act(Executor& exec)
    {
      return std::unique_ptr<Deliverable>();
    }
  } // namespace prot
} // namespace logsvc
