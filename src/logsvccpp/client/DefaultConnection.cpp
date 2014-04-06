/* Source file created: 2013-05-12

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

#include "logsvccpp/client/DefaultConnection.h"

#include "logsvccpp/SocketError.h"
#include "log/Deliverable.h"
#include "log/Receivable.h"
#include "log/ReceivableFactory.h"
#include "log/Message.h"
#include "network/Socket.h"
#include <iostream>

namespace logsvc
{
  namespace client
  {

    DefaultConnection::DefaultConnection(std::unique_ptr<network::Socket> socket,
                                         std::unique_ptr<prot::ReceivableFactory> factory) :
      socket(std::move(socket)),
      socket_mutex(new std::mutex),
      receivable_factory(std::move(factory)),
      current_receivable(),
      current_promise()
    {
      this->socket->keep_alive();
    }

    DefaultConnection::~DefaultConnection()
    {
      socket.reset();

      for (std::thread& t : threads)
      {
        if (t.joinable())
          t.join();
      }
    }

    std::future<std::unique_ptr<prot::Receivable>>
    DefaultConnection::send(const prot::Deliverable& deliverable)
    {
      std::promise<std::unique_ptr<prot::Receivable>> the_promise;
      auto the_future = the_promise.get_future();
      std::thread t(&DefaultConnection::serialized_send_message, this,
                    deliverable.get_header() + deliverable.get_payload(),
                    std::move(the_promise));
      threads.push_back(std::move(t));
      return the_future;
    }

    void DefaultConnection::bytes_received(const std::string& bytes)
    {
      if (!current_receivable)
      {
        current_receivable = receivable_factory->create(bytes);
        if (socket)
          socket->async_read(*this, current_receivable->get_payload_length());
      }
      else
      {
        current_receivable->read_payload(bytes);
        current_promise.set_value(std::move(current_receivable));
        if (socket)
          socket->keep_alive();

        socket_mutex->unlock();
      }
    }

    void DefaultConnection::write_succeeded()
    {
      if (socket)
        socket->async_read(*this, 12);
    }

    void DefaultConnection::error_occurred(const std::string& message)
    {
      current_receivable.reset();
      current_promise.set_exception(std::make_exception_ptr(SocketError(message)));
      if (socket)
        socket->keep_alive();

      socket_mutex->unlock();
    }

    void DefaultConnection::
    serialized_send_message(const std::string& data,
                            std::promise<std::unique_ptr<prot::Receivable>> promise)
    {
      try {
        if (!socket)
          return;

        socket_mutex->lock();
        current_promise = std::move(promise);
        if (socket)
        {
          socket->async_write(*this, data);
        }
      }
      catch (const std::exception& e)
      {
        std::cerr << "serialized_send_message caught exception: " << e.what() << std::endl;
      }
    }
  } // namespace client
} // namespace logsvc
