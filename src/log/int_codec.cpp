/* Source file created: 2013-04-06

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

#include "log/int_codec.h"
#include <stdexcept>

namespace logsvc
{
  namespace prot
  {

    std::string encode_32bit_int(std::size_t integer)
    {
      if (sizeof(std::size_t) > 4)
        if (integer > 0xFFFFFFFF)
          throw std::logic_error("encode_32bit_int can not encode integers "
                                 "larger than 32 bit.");

      std::string encoded;
      for (int i = 0; i < 4; ++i)
        encoded.push_back((integer >> (i * 8)) & 0xFF);

      return encoded;
    }

    std::size_t decode_32bit_int(const std::string& encoded)
    {
      if (encoded.size() != 4)
        throw std::logic_error("decode_32bit_int requires string size 4.");

      std::size_t decoded(0);
      for (int i = 0; i < 4; ++i)
        decoded |= (static_cast<std::size_t>(encoded[i]) & 0xff) << (i * 8);

      return decoded;
    }

  }
} // namespace logsvc
