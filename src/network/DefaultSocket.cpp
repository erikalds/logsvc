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
#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_array.hpp>

namespace network
{

  DefaultSocket::DefaultSocket(boost::asio::io_service& io_service) :
    the_socket(io_service),
    state_listeners(),
    write_queue_mutex(),
    write_queue(),
    io_service(io_service),
    idle_work(),
    write_queue_active(false)
  {
  }

  DefaultSocket::~DefaultSocket()
  {
    if (the_socket.is_open())
    {
      std::clog << "INFO [DefaultSocket]: Closing socket..." << std::endl;
      boost::system::error_code error;
      the_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
      if (error)
        std::cerr << "ERROR [DefaultSocket]: Error occurred during socket shutdown: " << error.message() << std::endl;
      the_socket.close(error);
      if (error)
        std::cerr << "ERROR [DefaultSocket]: Error occurred during socket close: " << error.message() << std::endl;
    }
    else
    {
      std::clog << "INFO [DefaultSocket]: No need to close the socket, it was already closed." << std::endl;
    }
  }

  void DefaultSocket::async_read(SocketListener& listener, std::size_t read_bytes)
  {
    std::clog << "INFO [DefaultSocket]: async_read " << read_bytes << " bytes..." << std::endl;
    boost::shared_array<char> buf(new char[read_bytes]);
    boost::asio::async_read(the_socket,
                            boost::asio::buffer(buf.get(), read_bytes),
                            boost::bind(&DefaultSocket::handle_read, this, buf,
                                        &listener, _1, _2));
    idle_work.reset();
  }

  void DefaultSocket::handle_read(boost::shared_array<char> buf, SocketListener* listener,
                                  const boost::system::error_code& error,
                                  std::size_t bytes_received)
  {
    if (!error)
    {
      std::string bytes(buf.get(), bytes_received);
      std::clog << "INFO [DefaultSocket]: received " << bytes_received << " bytes: \"" << bytes << "\"" << std::endl;
      listener->bytes_received(bytes);
    }
    else
    {
      std::clog << "ERROR [DefaultSocket]: error occurred during read: " << error.message() << std::endl;
      listener->error_occurred(error.message());
      notify_state_listeners_of_connection_lost();
    }
  }

  void DefaultSocket::async_write(SocketListener& listener, const std::string& data)
  {
    add_to_write_queue(data, &listener);
    write_front();
    idle_work.reset();
  }

  void DefaultSocket::add_socket_state_listener(SocketStateListener* listener)
  {
    state_listeners.insert(listener);
  }

  void DefaultSocket::remove_socket_state_listener(SocketStateListener* listener)
  {
    state_listeners.erase(listener);
  }

  void DefaultSocket::keep_alive()
  {
    idle_work.reset(new boost::asio::io_service::work(io_service));
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

  void DefaultSocket::add_to_write_queue(const std::string& msg, SocketListener* listener)
  {
    std::lock_guard<std::mutex> guard(write_queue_mutex);
    write_queue.push_back(std::pair<std::string, SocketListener*>(msg, listener));
  }

  void DefaultSocket::pop_write_queue()
  {
    std::lock_guard<std::mutex> guard(write_queue_mutex);
    assert(!write_queue.empty());
    write_queue.pop_front();
  }

  const std::pair<std::string, SocketListener*>& DefaultSocket::front_of_write_queue()
  {
    std::lock_guard<std::mutex> guard(write_queue_mutex);
    return write_queue.front();
  }

  bool DefaultSocket::is_write_queue_empty() const
  {
    std::lock_guard<std::mutex> guard(write_queue_mutex);
    return write_queue.empty();
  }

  void DefaultSocket::handle_write(const boost::system::error_code& error,
                                   SocketListener* listener)
  {
    pop_write_queue();
    write_queue_active = false;

    if (error)
    {
      std::clog << "ERROR [DefaultSocket]: error occurred during write: " << error.message() << std::endl;
      listener->error_occurred(error.message());
      notify_state_listeners_of_connection_lost();
      return;
    }

    std::clog << "INFO [DefaultSocket]: write succeeded." << std::endl;
    listener->write_succeeded();

    if (is_write_queue_empty())
      return;

    write_front();
  }

  void DefaultSocket::write_front()
  {
    bool active_before_exchanged = write_queue_active.exchange(true);
    if (active_before_exchanged)
      return;

    const std::pair<std::string, SocketListener*>& to_write = front_of_write_queue();
    std::clog << "INFO [DefaultSocket]: writing \"" << to_write.first << "\"" << std::endl;
    boost::asio::async_write(the_socket,
                             boost::asio::buffer(to_write.first.data(),
                                                 to_write.first.length()),
                             [&](const boost::system::error_code& error,
                                 std::size_t /*bytes_written*/)
                             { handle_write(error, to_write.second); });
  }


} // namespace network
