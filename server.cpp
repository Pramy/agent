#include <string>
#include "base_server.h"

int main() {
  BaseServer server;
  server.AddBeforeReadTasks([](BaseServer::SocketFD sock) {
    timeval tv{};
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  });
  server.AddAfterReadTasks([](BaseServer::SocketFD sock, const std::string &msg) {
    std::cout << msg << std::endl;
  });


  using socket_fd = BaseServer::SocketFD;
  socket_fd accept_socket = server.CreateServer("0.0.0.0", 8081);
  while(true) {
    sockaddr_in remote_addr{};
    socklen_t len;
    socket_fd client_fd = server.Accept(accept_socket, remote_addr, len);
    if(client_fd > 0) {

      std::string msg = server.Read(client_fd);
      socket_fd remote_fd = server.CreateClient("www.baidu.com", 80);

      server.Write(remote_fd, msg);
      std::string rec = server.Read(remote_fd);
      server.Write(client_fd, rec);
      server.Close(client_fd);
      server.Close(remote_fd);
    }
  }
  return 0;
}