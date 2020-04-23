#include <iostream>
#include <string>
#include "socketutil.h"

int main(int argh, char * argv[]) {

 SocketUtil::AddAfterCreateSocket([](const SocketUtil::SocketFD &sock){
// #ifdef DEBUG
    // PRINT("set time out");
// #endif
   timeval tv{};
   tv.tv_sec = 0;
   tv.tv_usec = 100000;
   setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
 });

  using socket_fd = SocketUtil::SocketFD;
  socket_fd accept_socket = SocketUtil::CreateServer("0.0.0.0", 8081);
  while(true) {
    sockaddr_in remote_addr{};
    socklen_t len;
    socket_fd client_fd = SocketUtil::Accept(accept_socket, remote_addr, len);
    if(client_fd > 0) {

      std::string msg = SocketUtil::Read(client_fd);
      socket_fd remote_fd = SocketUtil::CreateClient("www.baidu.com", 80);

      SocketUtil::Write(remote_fd, msg);
      std::string rec = SocketUtil::Read(remote_fd);
      SocketUtil::Write(client_fd, rec);
      SocketUtil::Close(client_fd);
    }
  }
  return 0;
}