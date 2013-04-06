/* Source file created: 2013-04-01

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

#include "log/create_header.h"
#include "log/int_codec.h"
#include <stdexcept>

namespace logsvc
{
  namespace prot
  {

    std::string create_header(const std::string& type, std::size_t length)
    {
      if (type.size() != 4)
        throw std::logic_error("Protocol object type must be 4 characters long");

      return "logs" + type + encode_32bit_int(length);
    }

  } // namespace prot
} // namespace logsvc
