/* Source file created: 2013-05-11

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

#include "logsvccpp/OutStream.h"

#include <streambuf>
#include "logsvccpp/Log.h"

namespace logsvc
{

  namespace {

    class LogBuf : public std::basic_streambuf<char>
    {
    public:
      LogBuf(Log& log) : log(log) {}

      virtual int sync()
      {
        log.logln(buffer);
        buffer.clear();
        return 0;
      }
      virtual int_type
      overflow(int_type ch = std::basic_streambuf<char>::traits_type::eof())
      {
        buffer.push_back(ch);
        assert(std::basic_streambuf<char>::traits_type::eof() != 1);
        return 1;
      }

      virtual std::streamsize xsputn(const char_type* s,
                                     std::streamsize count)
      {
        buffer.insert(buffer.end(), s, s + count);
        return count;
      }

    private:
      Log& log;
      std::string buffer;
    };

  }

  OutStream::OutStream(Log& log) :
    std::basic_ostream<char>(new LogBuf(log)),
    the_rdbuf(rdbuf())
  {
  }

} // namespace logsvc
