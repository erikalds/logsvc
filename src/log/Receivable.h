#ifndef RECEIVABLE_H_
#define RECEIVABLE_H_

/* Header created: 2013-03-27

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

#include <memory>
#include <string>

namespace logsvc
{
  namespace prot
  {

    class ClientExecutor;
    class Deliverable;
    class Executor;

    class Receivable
    {
    public:
      virtual ~Receivable() = 0;

      virtual std::size_t get_payload_length() const = 0;
      virtual void read_payload(const std::string& payload) = 0;
      virtual std::unique_ptr<Deliverable> act(Executor& exec) = 0;
      virtual void act(ClientExecutor& exec) = 0;
      virtual bool is_final_message() const = 0;
    };

    inline Receivable::~Receivable() {}
  } // namespace prot
} // namespace logsvc

#endif // RECEIVABLE_H_
