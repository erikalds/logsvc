/* Source file created: 2013-03-27

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

#include "log/NotAcknowledged.h"

#include "log/ClientExecutor.h"
#include "log/create_header.h"

namespace logsvc
{
  namespace prot
  {

    NotAcknowledged::NotAcknowledged(std::size_t payload_length) :
      AbstractReceivable(payload_length),
      reason()
    {
    }

    NotAcknowledged::NotAcknowledged(const std::string& reason) :
      AbstractReceivable(reason.size()),
      reason(reason)
    {
    }

    std::string NotAcknowledged::get_reason() const
    {
      return reason;
    }

    std::string NotAcknowledged::get_header() const
    {
      return create_header("nack", reason.length());
    }

    std::string NotAcknowledged::get_payload() const
    {
      return reason;
    }

    void NotAcknowledged::read_payload(const std::string& payload)
    {
      reason = payload;
    }

    std::unique_ptr<Deliverable> NotAcknowledged::act(Executor& exec)
    {
      return std::unique_ptr<Deliverable>();
    }

    void NotAcknowledged::act(ClientExecutor& exec)
    {
      exec.set_error(reason);
    }

  } // namespace prot
} // namespace logsvc
