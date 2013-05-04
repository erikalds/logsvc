/* Source file created: 2013-05-04

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

#include "logsvccpp/client/Host.h"

#include "logsvccpp/client/ConnectionFactory.h"
#include "logsvccpp/client/SessionConnection.h"
#include "log/Client.h"
#include <boost/asio/ip/address.hpp>

namespace logsvc
{
  namespace client
  {
    namespace
    {

      boost::asio::ip::address get_my_ip()
      {
        return boost::asio::ip::address_v4();
      }

    } // anonymous namespace


    Host::Host(const std::string& appname, const ConnectionFactory& confac) :
      connection(confac.create_session())
    {
      connection->send(prot::Client(appname, get_my_ip()));
    }

    Host::~Host()
    {
    }

  } // namespace client
} // namespace logsvc
