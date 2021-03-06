#ifndef DUMMYCLIENTEXECUTOR_H_
#define DUMMYCLIENTEXECUTOR_H_

/* Header created: 2013-05-11

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

#include "log/ClientExecutor.h"
#include "log/ClientHandle.h"
#include "log/FileHandle.h"
#include <boost/test/unit_test.hpp>

namespace logsvc
{
  namespace mock
  {

    class DummyClientExecutor : public logsvc::prot::ClientExecutor
    {
    public:
      DummyClientExecutor() :
        file_handle(), error_string("NOT SET"), expected_calls_to_success(0) {}

      ~DummyClientExecutor()
      { BOOST_CHECK_EQUAL(expected_calls_to_success, 0); }

      virtual void set_file_handle(const prot::FileHandle& fh)
      { file_handle = fh; }

      virtual void set_error(const std::string& s)
      { error_string = s; }

      virtual void success()
      { --expected_calls_to_success; }
      void set_client_handle(const prot::ClientHandle& ch) override
      { client_handle = ch; }

      void expect_call_to_success()
      {
        ++expected_calls_to_success;
      }

      prot::FileHandle file_handle;
      std::string error_string;
      int expected_calls_to_success;
      prot::ClientHandle client_handle;
    };

  } // namespace mock
} // namespace logsvc

#endif // DUMMYCLIENTEXECUTOR_H_
