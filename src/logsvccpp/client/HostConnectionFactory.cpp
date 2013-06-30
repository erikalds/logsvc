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

#include "logsvccpp/client/HostConnectionFactory.h"

#include "logsvccpp/UnableToConnectError.h"
#include "logsvccpp/client/DefaultConnection.h"
#include "log/Client.h"
#include "log/ProtObjFactory.h"
#include "network/DefaultSocket.h"
#include <egen/make_unique.h>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <future>
#include <thread>

namespace logsvc
{
  namespace client
  {
    namespace
    {

      class IOServiceSingleton
      {
      public:
        static void make_sure_it_runs();

        static boost::asio::io_service& io_service();

      private:
        IOServiceSingleton() : my_io_service() {}
        ~IOServiceSingleton()
        {
          if (thread.joinable())
            thread.join();
        }

        static IOServiceSingleton& instance();

        boost::asio::io_service my_io_service;
        std::thread thread;
      };

      void IOServiceSingleton::make_sure_it_runs()
      {
        if (!instance().thread.joinable())
          instance().thread =
            std::thread([&]()
                        {
                          boost::system::error_code error;
                          instance().my_io_service.run(error);
                          std::cerr << "WARNING: The io_service.run() function returned";
                          if (error)
                            std::cerr << " with an error: " << error;
                          std::cerr << ".\n";
                        });
      }

      boost::asio::io_service& IOServiceSingleton::io_service()
      {
        return instance().my_io_service;
      }

      IOServiceSingleton& IOServiceSingleton::instance()
      {
        static IOServiceSingleton inst;
        return inst;
      }

    } // anonymous namespace

    HostConnectionFactory::HostConnectionFactory(const std::string& hostname_or_ip,
                                                 const std::string& local_ip) :
      hostname_or_ip(hostname_or_ip),
      local_ip(local_ip)
    {
    }

    std::unique_ptr<SessionConnection>
    HostConnectionFactory::create_session() const
    {
      boost::asio::ip::tcp::resolver resolver(IOServiceSingleton::io_service());
      boost::asio::ip::tcp::resolver::query query(hostname_or_ip, "5607");
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

      std::unique_ptr<network::DefaultSocket> socket =
        egen::make_unique<network::DefaultSocket>(IOServiceSingleton::io_service());
      std::promise<bool> connection_promise;
      std::future<bool> future_connection = connection_promise.get_future();
      boost::asio::async_connect(socket->asio_socket(), endpoint_iterator,
                                 [&](const boost::system::error_code& error,
                                     boost::asio::ip::tcp::resolver::iterator /*iterator*/)
                                 {
                                   std::cout << "Handling async_connect: " << error.message() << std::endl;
                                   if (error)
                                   {
                                     UnableToConnectError utce(error.message());
                                     std::exception_ptr exception
                                       = std::make_exception_ptr(utce);
                                     connection_promise.set_exception(exception);
                                   }
                                   else
                                   {
                                     socket->keep_alive();
                                     connection_promise.set_value(true);
                                   }
                                 });

      IOServiceSingleton::make_sure_it_runs();

      std::unique_ptr<prot::ReceivableFactory> recvfactory(new prot::ProtObjFactory);
      if (!future_connection.get())
        throw UnableToConnectError("Unspecified connect error.");

      return egen::make_unique<DefaultConnection>(std::move(socket),
                                                  std::move(recvfactory));
    }

    prot::Client
    HostConnectionFactory::create_client_info(const std::string& appname) const
    {
      return prot::Client(appname, boost::asio::ip::address::from_string(local_ip));
    }

  } // namespace client
} // namespace logsvc
