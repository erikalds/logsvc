/* Source file created: 2013-11-17

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

#include "log/Disconnect.h"

#include "log/Acknowledged.h"
#include "log/ClientHandle.h"
#include "log/Deliverable.h"
#include "log/Executor.h"
#include "log/NotAcknowledged.h"

namespace logsvc
{
  namespace  prot
  {
    Disconnect::Disconnect() :
      AbstractReceivable(4),
      client()
    {
    }

    Disconnect::Disconnect(const ClientHandle& ch) :
      AbstractReceivable(ch.get_payload_length()),
      client(new ClientHandle(ch))
    {
    }

    Disconnect::~Disconnect()
    {
    }

    void Disconnect::read_payload(const std::string& payload)
    {
      client.reset(new ClientHandle);
      client->read_payload(payload);
    }

    std::unique_ptr<Deliverable> Disconnect::act(Executor& exec)
    {
      assert(client != nullptr);
      try
      {
        exec.disconnect_client(*client);
        return std::unique_ptr<Deliverable>(new Acknowledged);
      }
      catch (const std::exception& e)
      {
        return std::unique_ptr<Deliverable>(new NotAcknowledged(e.what()));
      }
    }

    void Disconnect::act(ClientExecutor&)
    {
    }

    bool Disconnect::is_final_message() const
    {
      return true;
    }

  } // namespace prot
} // namespace logsvc
