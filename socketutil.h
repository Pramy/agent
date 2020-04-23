//
// Created by tuffy on 2020/4/21.
//

#ifndef AGENT_SOCKETUTIL_H
#define AGENT_SOCKETUTIL_H
#define PRINT(a) (std::cout << a << std::endl)
#define ERROR_SOCKET (-1)

#include <netdb.h>
#include <string>
#include <vector>

class SocketUtil{
public:
  typedef int SocketFD;

  static SocketFD CreateServer(const std::string &host, const int &port);

  static SocketFD CreateClient(const std::string &host, const int &port);

  static SocketFD Accept(SocketFD &sock,sockaddr_in &remote_addr, socklen_t &len);

  static ssize_t Write(SocketFD &socket_fd, const std::string &msg);

  static std::string Read(SocketFD &socket_fd);

  static int Close(SocketFD &socket_fd);

  static void AddAfterCreateSocket(const std::function<void (const SocketFD&)> &fn);

private:

  static bool Connect(SocketFD &sock, addrinfo& addr);

  static bool BindAndListen(SocketFD &sock, addrinfo& addr);

  static void SetOption(const SocketFD &sock);

  static SocketFD CreateSocket(const std::string &host,
                               const int &port,
                               const std::function<bool(int&, addrinfo&)> &fn);

private:

  static std::vector<std::function<void (const SocketFD&)> > after_create_socket;
};



#endif //AGENT_SOCKETUTIL_H
