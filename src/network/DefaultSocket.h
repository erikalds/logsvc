#ifndef DEFAULTSOCKET_H_
#define DEFAULTSOCKET_H_

/* Header created: 2013-05-03

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

#include "network/Socket.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_array.hpp>
#include <atomic>
#include <deque>
#include <mutex>
#include <set>

namespace network
{

  class DefaultSocket : public Socket
  {
  public:
    DefaultSocket(boost::asio::io_service& io_service);
    ~DefaultSocket();

    virtual void async_read(SocketListener& listener, std::size_t read_bytes);
    virtual void async_write(SocketListener& listener, const std::string& data);

    virtual void add_socket_state_listener(SocketStateListener* listener);
    virtual void remove_socket_state_listener(SocketStateListener* listener);

    virtual void keep_alive();

    boost::asio::ip::tcp::socket& asio_socket();

  private:
    void handle_read(boost::shared_array<char> buf, SocketListener* listener,
                     const boost::system::error_code& error,
                     std::size_t bytes_received);

    void notify_state_listeners_of_connection_lost();

    void add_to_write_queue(const std::string& msg, SocketListener* listener);
    void pop_write_queue();
    const std::pair<std::string, SocketListener*>& front_of_write_queue();
    bool is_write_queue_empty() const;
    void write_front();
    void handle_write(const boost::system::error_code& error,
                      SocketListener* listener);

    boost::asio::ip::tcp::socket the_socket;
    std::set<SocketStateListener*> state_listeners;
    mutable std::mutex write_queue_mutex;
    std::deque<std::pair<std::string, SocketListener*>> write_queue;
    boost::asio::io_service& io_service;
    std::unique_ptr<boost::asio::io_service::work> idle_work;
    std::atomic<bool> write_queue_active;
    std::atomic<bool> were_dying;
  };

} // namespace network

#endif // DEFAULTSOCKET_H_
