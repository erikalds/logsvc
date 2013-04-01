/* Source file created: 2013-03-27

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

#include "log/NotAcknowledged.h"

namespace logsvc
{
  namespace prot
  {

    NotAcknowledged::NotAcknowledged(const std::string& reason) :
      reason(reason)
    {
    }

    std::string NotAcknowledged::get_reason() const
    {
      return reason;
    }

    std::string NotAcknowledged::get_header() const
    {
      std::string header("logsnack\x00\x00\x00\x00", 12);
      header[8] = reason.length() & 0xff;
      header[9] = (reason.length() >> 8) & 0xff;
      header[10] = (reason.length() >> 16) & 0xff;
      header[11] = (reason.length() >> 24)& 0xff;
      return header;
    }

    std::string NotAcknowledged::get_payload() const
    {
      return reason;
    }

  } // namespace prot
} // namespace logsvc