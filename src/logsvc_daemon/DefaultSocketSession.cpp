/* Source file created: 2013-03-25

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

#include "logsvc_daemon/DefaultSocketSession.h"

#include "logsvc_daemon/SocketSessionListener.h"
#include "log/Deliverable.h"
#include "log/Executor.h"
#include "log/Receivable.h"
#include "log/ReceivableFactory.h"
#include "network/Socket.h"

namespace logsvc
{
  namespace daemon
  {
    namespace constants
    {
      const static std::size_t header_length = 12;
    } // namespace constants

    DefaultSocketSession::DefaultSocketSession(std::unique_ptr<network::Socket> socket,
                                               std::unique_ptr<prot::Executor> exec,
                                               std::unique_ptr<prot::ReceivableFactory> rf) :
      the_socket(std::move(socket)),
      the_receivable_factory(std::move(rf)),
      current_receivable(nullptr),
      executor(std::move(exec))
    {
      the_socket->add_socket_state_listener(this);
    }

    DefaultSocketSession::~DefaultSocketSession()
    {
    }

    void DefaultSocketSession::start_listen()
    {
      std::clog << "INFO [DefaultSocketSession]: listening for header..." << std::endl;
      the_socket->async_read(*this, constants::header_length);
    }

    void DefaultSocketSession::add_socket_session_listener(SocketSessionListener* l)
    {
      listeners.insert(l);
    }

    void DefaultSocketSession::remove_socket_session_listener(SocketSessionListener* l)
    {
      listeners.erase(l);
    }

    void DefaultSocketSession::bytes_received(const std::string& bytes)
    {
      std::clog << "INFO [DefaultSocketSession]: received " << bytes.size() << " bytes." << std::endl;
      if (!current_receivable)
      {
        current_receivable = the_receivable_factory->create(bytes);
        the_socket->async_read(*this, current_receivable->get_payload_length());
      }
      else
      {
        current_receivable->read_payload(bytes);
        std::unique_ptr<prot::Deliverable> deliverable =
          current_receivable->act(*executor);
        the_socket->async_write(*this,
                                deliverable->get_header() + deliverable->get_payload());
      }
    }

    void DefaultSocketSession::write_succeeded()
    {
      std::clog << "INFO [DefaultSocketSession]: write succeeded." << std::endl;
      listen_for_new_header();
    }

    void DefaultSocketSession::error_occurred(const std::string& message)
    {
      std::clog << "ERROR [DefaultSocketSession]: " << message << std::endl;
      listen_for_new_header();
    }

    void DefaultSocketSession::connection_lost(network::Socket* /*socket*/)
    {
      for (SocketSessionListener* listener : listeners)
        listener->connection_lost(this);
    }

    void DefaultSocketSession::listen_for_new_header()
    {
      current_receivable.reset();
      start_listen();
    }

  } // namespace daemon
} // namespace logsvc
