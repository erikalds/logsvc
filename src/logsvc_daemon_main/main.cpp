#include "logsvc_daemon/SessionInitiator.h"
#include "logsvc_daemon/DefaultSessionFactory.h"
#include "network/DefaultSocketAcceptor.h"

int main()
{
  logsvc::daemon::DefaultSessionFactory session_factory;
  network::DefaultSocketAcceptor socket_acceptor;
  logsvc::daemon::SessionInitiator initiator(session_factory, socket_acceptor);
  socket_acceptor.run();
  return 0;
}
