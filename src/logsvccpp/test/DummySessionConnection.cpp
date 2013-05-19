/* Source file created: 2013-05-09

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

#include "logsvccpp/test/DummySessionConnection.h"

#include "log/ClientHandle.h"
#include "log/Deliverable.h"
#include "log/FileHandle.h"
#include "log/NotAcknowledged.h"
#include "log/ProtObjFactory.h"
#include <egen/lookup.h>
#include <egen/make_unique.h>
#include <boost/test/unit_test.hpp>

namespace logsvc
{
  namespace mock
  {

    DummySessionConnection::DummySessionConnection(const DSCKilledListener* listener) :
      client_name("UNSET"),
      client_address("UNSET"),
      open_file_error(),
      listener(listener)
    {
    }

    DummySessionConnection::~DummySessionConnection()
    {
      if (listener)
        listener->connection_killed(this);
    }

    std::future<std::unique_ptr<prot::Receivable>>
    DummySessionConnection::send(const prot::Deliverable& deliverable)
    {
      prot::ProtObjFactory factory;
      std::unique_ptr<prot::Receivable> recv =
        factory.create(deliverable.get_header());
      BOOST_REQUIRE(recv != nullptr);
      recv->read_payload(deliverable.get_payload());

      try
      {
        std::unique_ptr<prot::Deliverable> reply = recv->act(*this);
        recv = factory.create(reply->get_header());
        recv->read_payload(reply->get_payload());
        std::promise<std::unique_ptr<prot::Receivable>> promise;
        auto future = promise.get_future();
        promise.set_value(std::move(recv));
        return future;
      }
      catch (const std::exception& e)
      {
        std::promise<std::unique_ptr<prot::Receivable>> promise;
        auto future = promise.get_future();
        promise.set_value(egen::make_unique<prot::NotAcknowledged>(e.what()));
        return future;
      }
    }

    prot::FileHandle
    DummySessionConnection::open_file(const boost::filesystem::path& filename)
    {
      if (!open_file_error.empty())
        throw std::runtime_error(open_file_error);

      opened_files.push_back(filename);
      prot::FileHandle fh(opened_files.size() - 1);
      fh_to_idx[fh] = opened_files.size() - 1;
      return fh;
    }

    void DummySessionConnection::close_file(const prot::FileHandle& fh)
    {
      std::size_t idx = lookup_idx(fh);
      opened_files[idx] = "CLOSED_FILE(" + opened_files[idx].string() + ")";
    }

    void DummySessionConnection::write_message(const logsvc::prot::FileHandle& fh,
                                               const std::string& message)
    {
      if (!write_message_error.empty())
        throw std::runtime_error(write_message_error);

      std::size_t idx = lookup_idx(fh);
      file_contents[opened_files[idx]] += message + "\n";
    }

    prot::ClientHandle
    DummySessionConnection::set_client_info(const std::string& name,
                                            const std::string& address)
    {
      client_name = name;
      client_address = address;
      return prot::ClientHandle();
    }

    std::size_t DummySessionConnection::lookup_idx(const logsvc::prot::FileHandle fh) const
    {
      std::size_t idx = egen::lookup(fh, fh_to_idx,
                                     std::numeric_limits<std::size_t>::max());
      BOOST_REQUIRE_NE(idx, std::numeric_limits<std::size_t>::max());
      return idx;
    }

  }
} // namespace logsvc
