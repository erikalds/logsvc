/* Source file created: 2013-04-28

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

#include "network/DefaultSocketAcceptor.h"

#include "network/DefaultSocket.h"
#include "network/SocketAcceptListener.h"

using namespace boost::asio;

namespace network
{

  DefaultSocketAcceptor::DefaultSocketAcceptor() :
    io_service(),
    acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), 5607))
  {
  }

  void DefaultSocketAcceptor::async_accept(SocketAcceptListener& listener)
  {
    std::unique_ptr<DefaultSocket> api_socket(new DefaultSocket(io_service));
    acceptor.async_accept(api_socket->asio_socket(),
                          [&](const boost::system::error_code& error)
                          {
                            if (!error)
                              listener.accept_requested(std::move(api_socket));
                            else
                              listener.error_occurred(error.message());
                          });
  }

  void DefaultSocketAcceptor::run()
  {
    io_service.run();
  }

} // namespace network
