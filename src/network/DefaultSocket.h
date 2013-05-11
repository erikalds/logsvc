#ifndef DEFAULTSOCKET_H_
#define DEFAULTSOCKET_H_

/* Header created: 2013-05-03

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

#include "network/Socket.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <deque>
#include <set>

namespace network
{

  class DefaultSocket : public Socket
  {
  public:
    DefaultSocket(boost::asio::io_service& io_service);

    virtual void async_read(SocketListener& listener, std::size_t read_bytes);
    virtual void async_write(const std::string& data);

    virtual void add_socket_state_listener(SocketStateListener* listener);
    virtual void remove_socket_state_listener(SocketStateListener* listener);

    boost::asio::ip::tcp::socket& asio_socket();

  private:
    void notify_state_listeners_of_connection_lost();

    void add_to_write_queue(const std::string& msg);
    void pop_write_queue();
    std::string front_of_write_queue();
    bool is_write_queue_empty() const;
    void write_front();
    void handle_write(const boost::system::error_code& error);

    boost::asio::ip::tcp::socket the_socket;
    std::set<SocketStateListener*> state_listeners;
    std::deque<std::string> write_queue;
  };

} // namespace network

#endif // DEFAULTSOCKET_H_