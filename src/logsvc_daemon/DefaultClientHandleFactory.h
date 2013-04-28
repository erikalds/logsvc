#ifndef DEFAULTCLIENTHANDLEFACTORY_H_
#define DEFAULTCLIENTHANDLEFACTORY_H_

/* Header created: 2013-04-28

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

#include "logsvc_daemon/ClientHandleFactory.h"
#include <atomic>

namespace logsvc
{
  namespace daemon
  {

    class DefaultClientHandleFactory : public ClientHandleFactory
    {
    public:
      DefaultClientHandleFactory();

      virtual prot::ClientHandle generate_client_handle();

    private:
      std::atomic<unsigned int> handle_counter;
    };

  } // namespace daemon
} // namespace logsvc

#endif // DEFAULTCLIENTHANDLEFACTORY_H_
