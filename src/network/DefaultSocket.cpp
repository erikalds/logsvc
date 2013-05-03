/* Source file created: 2013-05-03

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

#include "network/DefaultSocket.h"

#include "network/SocketListener.h"
#include "network/SocketStateListener.h"
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/scoped_array.hpp>

namespace network
{

  DefaultSocket::DefaultSocket(boost::asio::io_service& io_service) :
    the_socket(io_service)
  {
  }

  void DefaultSocket::async_read(SocketListener& listener, std::size_t read_bytes)
  {
    boost::scoped_array<char> buf(new char[read_bytes]);
    boost::asio::async_read(the_socket,
                            boost::asio::buffer(buf.get(), read_bytes),
                            [&](const boost::system::error_code& error,
                                std::size_t bytes_received)
                            {
                              if (!error)
                              {
                                std::string bytes(buf.get(), bytes_received);
                                listener.receive_bytes(bytes);
                              }
                              else
                              {
                                listener.error_occurred(error.message());
                                notify_state_listeners_of_connection_lost();
                              }
                            });
  }

  void DefaultSocket::async_write(const std::string& data)
  {
    add_to_write_queue(data);
    write_front();
  }

  void DefaultSocket::add_socket_state_listener(SocketStateListener* listener)
  {
    state_listeners.insert(listener);
  }

  void DefaultSocket::remove_socket_state_listener(SocketStateListener* listener)
  {
    state_listeners.erase(listener);
  }

  boost::asio::ip::tcp::socket& DefaultSocket::asio_socket()
  {
    return the_socket;
  }

  void DefaultSocket::notify_state_listeners_of_connection_lost()
  {
    for (SocketStateListener* listener : state_listeners)
      listener->connection_lost(this);
  }

  void DefaultSocket::add_to_write_queue(const std::string& msg)
  {
    write_queue.push_back(msg);
  }

  void DefaultSocket::pop_write_queue()
  {
    write_queue.pop_front();
  }

  std::string DefaultSocket::front_of_write_queue()
  {
    return write_queue.front();
  }

  bool DefaultSocket::is_write_queue_empty() const
  {
    return write_queue.empty();
  }

  void DefaultSocket::handle_write(const boost::system::error_code& error)
  {
    if (error)
    {
      notify_state_listeners_of_connection_lost();
      return;
    }

    pop_write_queue();
    if (is_write_queue_empty())
      return;

    write_front();
  }

  void DefaultSocket::write_front()
  {
    std::string to_write = front_of_write_queue();
    boost::asio::async_write(the_socket,
                             boost::asio::buffer(to_write.data(), to_write.length()),
                             [&](const boost::system::error_code& error,
                                 std::size_t /*bytes_written*/)
                             { handle_write(error); });
  }


} // namespace network
