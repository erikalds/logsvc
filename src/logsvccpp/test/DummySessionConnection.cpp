/* Source file created: 2013-05-09

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

#include "logsvccpp/test/DummySessionConnection.h"

#include "log/ClientHandle.h"
#include "log/Deliverable.h"
#include "log/FileHandle.h"
#include "log/ProtObjFactory.h"
#include <egen/lookup.h>
#include <boost/test/unit_test.hpp>

namespace logsvc
{
  namespace mock
  {

    DummySessionConnection::DummySessionConnection(const DSCKilledListener* listener) :
      client_name("UNSET"),
      client_address("UNSET"),
      listener(listener)
    {
    }

    DummySessionConnection::~DummySessionConnection()
    {
      if (listener)
        listener->connection_killed(this);
    }

    std::unique_ptr<logsvc::prot::Receivable>
    DummySessionConnection::send(const logsvc::prot::Deliverable& deliverable)
    {
      logsvc::prot::ProtObjFactory factory;
      std::unique_ptr<logsvc::prot::Receivable> recv =
        factory.create(deliverable.get_header());
      BOOST_REQUIRE(recv != nullptr);
      recv->read_payload(deliverable.get_payload());

      std::unique_ptr<logsvc::prot::Deliverable> reply = recv->act(*this);
      recv = factory.create(reply->get_header());
      recv->read_payload(reply->get_payload());
      return std::move(recv);
    }

    logsvc::prot::FileHandle
    DummySessionConnection::open_file(const boost::filesystem::path& filename)
    {
      opened_files.push_back(filename);
      logsvc::prot::FileHandle fh(opened_files.size() - 1);
      fh_to_idx[fh] = opened_files.size() - 1;
      return fh;
    }

    void DummySessionConnection::close_file(const logsvc::prot::FileHandle& fh)
    {
      std::size_t idx = egen::lookup(fh, fh_to_idx,
                                     std::numeric_limits<std::size_t>::max());
      BOOST_REQUIRE_NE(idx, std::numeric_limits<std::size_t>::max());
      opened_files[idx] = "CLOSED_FILE(" + opened_files[idx].string() + ")";
    }

    logsvc::prot::ClientHandle
    DummySessionConnection::set_client_info(const std::string& name,
                                            const std::string& address)
    {
      client_name = name;
      client_address = address;
      return logsvc::prot::ClientHandle();
    }
  }
} // namespace logsvc
