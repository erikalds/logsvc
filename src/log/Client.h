#ifndef CLIENT_H_
#define CLIENT_H_

/* Header created: 2013-03-09

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

#include <boost/asio/ip/address.hpp>
#include <string>

namespace logsvc
{
  namespace prot
  {

    class Client
    {
    public:
      Client(const std::string& name);
      Client(const std::string& name, const boost::asio::ip::address& ip);

      std::string get_name() const;
      boost::asio::ip::address get_ip_address() const;

    private:
      std::string its_name;
      boost::asio::ip::address its_ip;
    };

  } // namespace prot
} // namespace logsvc

#endif // CLIENT_H_
