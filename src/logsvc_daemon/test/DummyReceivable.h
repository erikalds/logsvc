#ifndef DUMMYRECEIVABLE_H_
#define DUMMYRECEIVABLE_H_

/* Header created: 2013-05-20

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

#include "log/Executor.h"
#include "log/FileHandle.h"
#include "log/NotAcknowledged.h"
#include "log/Receivable.h"
#include <boost/test/unit_test.hpp>

namespace mock {

  class DummyReceivable : public logsvc::prot::Receivable
  {
  public:
    DummyReceivable(const std::string& expected_payload) :
      expected_payload(expected_payload), received_payload(false) {}
    ~DummyReceivable()
    {
      if (!expected_payload.empty())
        BOOST_CHECK(received_payload == true);
    }

    virtual std::size_t get_payload_length() const
    {
      return expected_payload.size();
    }

    virtual void read_payload(const std::string& payload)
    {
      BOOST_CHECK_EQUAL(expected_payload, payload);
      received_payload = true;
    }
    virtual std::unique_ptr<logsvc::prot::Deliverable> act(logsvc::prot::Executor& exec)
    {
      exec.write_message(logsvc::prot::FileHandle(0x42), "Hello");
      return std::unique_ptr<logsvc::prot::Deliverable>(new logsvc::prot::NotAcknowledged("fail"));
    }

    virtual void act(logsvc::prot::ClientExecutor&) {}

  private:
    std::string expected_payload;
    bool received_payload;
  };

} // namespace mock

#endif // DUMMYRECEIVABLE_H_
