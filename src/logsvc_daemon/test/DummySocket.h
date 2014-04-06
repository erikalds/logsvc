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
#include <egen/make_unique.h>
#include <boost/test/unit_test.hpp>
#include <set>
#include <queue>

namespace mock
{

  class DummySocket;

  class DummySocketKilledListener
  {
  public:
    virtual ~DummySocketKilledListener() = 0;

    virtual void socket_killed(const DummySocket* socket) = 0;
  };

  inline DummySocketKilledListener::~DummySocketKilledListener() {}

  class IOOperation
  {
  public:
    IOOperation() : perform_count(0) {}
    virtual ~IOOperation() { /*BOOST_CHECK_EQUAL(perform_count, 1);*/ }

    virtual void perform(const std::string& receive_bytes="") = 0;
    virtual void make_error_occur(const std::string& /*message*/)
    { BOOST_FAIL("This operation cannot fail."); }

  protected:
    int perform_count;
  };

  class CanFailOp : public IOOperation
  {
  public:
    CanFailOp(network::SocketListener* listener) : listener(listener) {}

    virtual void make_error_occur(const std::string& message)
    { ++perform_count; listener->error_occurred(message); }

  protected:
    network::SocketListener* listener;
  };

  class WriteOp : public CanFailOp
  {
  public:
    WriteOp(network::SocketListener* listener, const std::string& write_data,
            std::string& bytes_written) :
      CanFailOp(listener), write_data(write_data), bytes_written(bytes_written)
    {}

    virtual void perform(const std::string& receive_bytes)
    {
      ++perform_count;
      BOOST_CHECK_EQUAL(receive_bytes, "");
      bytes_written.append(write_data);
      listener->write_succeeded();
    }

    const std::string write_data;
    std::string& bytes_written;
  };

  class ReadOp : public CanFailOp
  {
  public:
    ReadOp(network::SocketListener* listener) : CanFailOp(listener) {}

    virtual void perform(const std::string& receive_bytes)
    {
      ++perform_count;
      BOOST_CHECK_NE(receive_bytes, "");
      listener->bytes_received(receive_bytes);
    }
  };

  class NoOp : public IOOperation
  {
  public:
    NoOp() { perform_count = 1; }
    virtual void perform(const std::string& receive_bytes)
    {
      BOOST_CHECK_EQUAL("", receive_bytes);
    }
  };

  class DummySocket : public network::Socket
  {
  public:
    explicit DummySocket(DummySocketKilledListener* kill_listener=nullptr) :
      async_read_call_count(0), async_read_byte_count(0), written_bytes(),
      op_queue(), kill_listener(kill_listener) { do_keep_alive(); }

    ~DummySocket()
    {
      if (kill_listener)
        kill_listener->socket_killed(this);
    }

    virtual void async_read(network::SocketListener& listener, std::size_t read_bytes)
    {
      BOOST_REQUIRE_GE(op_queue.size(), 1);
      remove_front_if_noop();
      op_queue.push(egen::make_unique<ReadOp>(&listener));
      ++async_read_call_count;
      async_read_byte_count = read_bytes;
    }

    void receive_bytes(const std::string& bytes)
    {
      BOOST_REQUIRE_GE(op_queue.size(), 1);
      IOOperation& op = *op_queue.front();
      op.perform(bytes);
      op_queue.pop();
    }

    virtual void async_write(network::SocketListener& listener, const std::string& data)
    {
      BOOST_REQUIRE_GE(op_queue.size(), 1);
      remove_front_if_noop();
      op_queue.push(egen::make_unique<WriteOp>(&listener, data, written_bytes));
    }

    void finish_write_op()
    {
      BOOST_REQUIRE_GE(op_queue.size(), 1);
      IOOperation& op = *op_queue.front();
      op.perform();
      op_queue.pop();
    }

    void make_error_occur(const std::string& msg)
    {
      BOOST_REQUIRE_GE(op_queue.size(), 1);
      IOOperation& op = *op_queue.front();
      op.make_error_occur(msg);
      op_queue.pop();
    }

    virtual void keep_alive()
    {
      BOOST_REQUIRE_GE(op_queue.size(), 1);
      remove_front_if_noop();
      do_keep_alive();
    }

    void do_keep_alive()
    {
      op_queue.push(egen::make_unique<NoOp>());
    }

    void remove_front_if_noop()
    {
      BOOST_REQUIRE_GE(op_queue.size(), 1);
      const IOOperation& op = *op_queue.front();
      if (dynamic_cast<const NoOp*>(&op))
        op_queue.pop();
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

    int async_read_call_count;
    std::size_t async_read_byte_count;
    std::string written_bytes;
    std::set<network::SocketStateListener*> listeners;
    std::queue<std::unique_ptr<IOOperation>> op_queue;
    DummySocketKilledListener* kill_listener;
  };

} // namespace mock

#endif // DUMMYSOCKET_H_
