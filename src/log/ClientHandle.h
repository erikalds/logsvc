#ifndef CLIENTHANDLE_H_
#define CLIENTHANDLE_H_

/* Header created: 2013-04-01

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

#include "log/Deliverable.h"
#include "log/AbstractReceivable.h"

namespace logsvc
{
  namespace prot
  {
    class ClientHandle : public Deliverable,
                         public AbstractReceivable
    {
    public:
      ClientHandle();
      explicit ClientHandle(unsigned int ch);

      virtual std::string get_header() const;
      virtual std::string get_payload() const;

      bool operator==(const ClientHandle& other) const;
      bool operator<(const ClientHandle& other) const;

      virtual void read_payload(const std::string& payload);
      virtual std::unique_ptr<Deliverable> act(Executor& exec);
      void act(ClientExecutor& exec) override;

    private:
      unsigned int ch;
      friend std::ostream& operator<<(std::ostream& out, const ClientHandle& ch);
    };

    inline bool operator!=(const ClientHandle& lhs, const ClientHandle& rhs)
    { return !(lhs == rhs); }
    inline bool operator<=(const ClientHandle& lhs, const ClientHandle& rhs)
    { return lhs < rhs || lhs == rhs; }
    inline bool operator>(const ClientHandle& lhs, const ClientHandle& rhs)
    { return !(lhs <= rhs); }
    inline bool operator>=(const ClientHandle& lhs, const ClientHandle& rhs)
    { return !(lhs < rhs); }
    std::ostream& operator<<(std::ostream& out, const ClientHandle& ch);
  } // namespace prot
} // namespace logsvc

#endif // CLIENTHANDLE_H_
