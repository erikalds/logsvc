/* Source file created: 2013-05-12

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

#include "logsvccpp/client/DefaultConnection.h"

#include "log/Deliverable.h"
#include "log/Receivable.h"
#include "network/Socket.h"

namespace logsvc
{
  namespace client
  {

    DefaultConnection::DefaultConnection(std::unique_ptr<network::Socket> socket) :
      socket(std::move(socket))
    {
    }

    std::future<std::unique_ptr<prot::Receivable>>
    DefaultConnection::send(const prot::Deliverable& deliverable)
    {
      socket->async_write(deliverable.get_header() + deliverable.get_payload());
      socket->async_read(*this, 0);
      std::promise<std::unique_ptr<prot::Receivable>> promise;
      return promise.get_future();
    }

    void DefaultConnection::receive_bytes(const std::string& bytes)
    {
    }

    void DefaultConnection::error_occurred(const std::string& message)
    {
    }

  } // namespace client
} // namespace logsvc
