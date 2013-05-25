#ifndef HOST_H_
#define HOST_H_

/* Header created: 2013-05-11

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

#include <boost/filesystem/path.hpp>
#include <string>
#include <memory>

namespace logsvc
{
  namespace client { class LocalClient; class RemoteLogFile; }

  class Host
  {
    friend class Log;

  public:
    Host(const std::string& appname, const std::string& hostname="localhost");

  private:
    std::unique_ptr<client::RemoteLogFile>
    open_remote(const boost::filesystem::path& file);

    client::LocalClient& get_connected_client();

    std::shared_ptr<client::LocalClient> local_client;
    std::string appname;
    std::string hostname;
  };

} // namespace logsvc

#endif // HOST_H_
