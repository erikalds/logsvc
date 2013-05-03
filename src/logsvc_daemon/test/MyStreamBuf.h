#ifndef MYSTREAMBUF_H_
#define MYSTREAMBUF_H_

/* Header created: 2013-05-03

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

#include <streambuf>

struct MyStreamBuf : std::basic_streambuf<char, std::char_traits<char> >
{
  std::ostringstream contents;

  virtual int_type overflow(int_type ch = std::char_traits<char>::eof())
  { contents << char(ch); assert(1 != std::char_traits<char>::eof()); return 1; }
  virtual std::streamsize xsputn(const char_type* s, std::streamsize count)
  { contents << std::string(s, count); return count; }
};

struct ClogInterceptor
{
  ClogInterceptor() : orig_rdbuf(nullptr), mybuf() {}

  virtual ~ClogInterceptor()
  {
    if (orig_rdbuf)
      std::clog.rdbuf(orig_rdbuf);
  }

  void intercept_clog()
  {
    std::clog.rdbuf(&mybuf);
  }

  std::string clog_contents()
  {
    return mybuf.contents.str();
  }

private:
  std::basic_streambuf<char, std::char_traits<char> >* orig_rdbuf;
  MyStreamBuf mybuf;
};

#endif // MYSTREAMBUF_H_

