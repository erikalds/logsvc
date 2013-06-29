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
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

using namespace boost::asio;

namespace network
{

  DefaultSocketAcceptor::DefaultSocketAcceptor() :
    io_service(),
    acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), 5607))
  {
  }

  namespace {

    void handle_accept(DefaultSocket* socket,
                       SocketAcceptListener* listener,
                       const boost::system::error_code& error)
    {
      std::unique_ptr<DefaultSocket> api_socket(socket);
      if (!error)
      {
        std::clog << "INFO [DefaultSocketAcceptor]: async accept succeeded." << std::endl;
        listener->accept_requested(std::move(api_socket));
      }
      else
      {
        std::clog << "INFO [DefaultSocketAcceptor]: async accept failed: "
                  << error.message() << std::endl;
        listener->error_occurred(error.message());
      }
    }

  } // anonymous namespace

  void DefaultSocketAcceptor::async_accept(SocketAcceptListener& listener)
  {
    std::unique_ptr<DefaultSocket> api_socket(new DefaultSocket(io_service));
    std::clog << "INFO [DefaultSocketAcceptor]: async accept is waiting..." << std::endl;
    boost::asio::ip::tcp::socket& asio_socket = api_socket->asio_socket();
    acceptor.async_accept(asio_socket,
                          boost::bind(handle_accept, api_socket.release(),
                                      &listener, boost::asio::placeholders::error));
  }

  void DefaultSocketAcceptor::run()
  {
    io_service.run();
    std::cerr << "ERROR [DefaultSocketAcceptor]: io_service.run has returned..." << std::endl;
  }

} // namespace network
