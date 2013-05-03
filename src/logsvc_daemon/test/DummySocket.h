#ifndef DUMMYSOCKET_H_
#define DUMMYSOCKET_H_

/* Header created: 2013-04-13

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
#include "network/SocketListener.h"
#include "network/SocketStateListener.h"
#include <boost/test/unit_test.hpp>
#include <set>

namespace mock
{

  class DummySocket : public network::Socket
  {
  public:
    DummySocket() : async_read_call_count(0), current_listener(nullptr), written_bytes() {}

    virtual void async_read(network::SocketListener& listener, std::size_t read_bytes)
    {
      BOOST_REQUIRE(current_listener == nullptr);
      current_listener = &listener;
      ++async_read_call_count;
      async_read_byte_count = read_bytes;
    }

    virtual void async_write(const std::string& data)
    {
      written_bytes += data;
    }

    void receive_bytes(const std::string& bytes)
    {
      BOOST_REQUIRE(current_listener != nullptr);
      network::SocketListener* this_listener = nullptr;
      std::swap(current_listener, this_listener);
      this_listener->receive_bytes(bytes);
    }

    virtual void add_socket_state_listener(network::SocketStateListener* listener)
    { listeners.insert(listener); }
    virtual void remove_socket_state_listener(network::SocketStateListener* listener)
    { listeners.insert(listener); }

    void kill_connection()
    {
      for (network::SocketStateListener* listener : listeners)
        listener->connection_lost(this);
    }

    void make_error_occur(const std::string& msg)
    {
      BOOST_REQUIRE(current_listener != nullptr);
      network::SocketListener* this_listener = nullptr;
      std::swap(current_listener, this_listener);
      this_listener->error_occurred(msg);
    }

    int async_read_call_count;
    std::size_t async_read_byte_count;
    network::SocketListener* current_listener;
    std::string written_bytes;
    std::set<network::SocketStateListener*> listeners;
  };

} // namespace mock

#endif // DUMMYSOCKET_H_
