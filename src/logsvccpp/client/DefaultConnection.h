#ifndef DEFAULTCONNECTION_H_
#define DEFAULTCONNECTION_H_

/* Header created: 2013-05-12

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

#include "logsvccpp/client/SessionConnection.h"

namespace network { class Socket; }

namespace logsvc
{
  namespace client
  {

    class DefaultConnection : public SessionConnection
    {
    public:
      DefaultConnection(std::unique_ptr<network::Socket>);

      virtual std::unique_ptr<prot::Receivable>
      send(const prot::Deliverable& deliverable);
    };

  } // namespace client
} // namespace logsvc

#endif // DEFAULTCONNECTION_H_
