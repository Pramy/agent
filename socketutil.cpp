//
// Created by tuffy on 2020/4/21.
//

#include "socketutil.h"

#include <sys/socket.h>
#include <unistd.h>


#ifdef DEBUG
#include <iostream>
#endif
#include <string>

template <class  T>
inline
void log(const T &msg){
#ifdef DEBUG
  std::cout << msg << std::endl;
#endif
}
std::vector<std::function<void (const SocketUtil::SocketFD&)> > SocketUtil::after_create_socket;

SocketUtil::SocketFD
SocketUtil::CreateServer(const std::string &host, const int &port){
  return SocketUtil::CreateSocket(host, port, SocketUtil::BindAndListen);
}

SocketUtil::SocketFD
SocketUtil::CreateClient(const std::string &host, const int &port){
  return SocketUtil::CreateSocket(host, port, SocketUtil::Connect);
}

bool SocketUtil::Connect(SocketFD &sock, addrinfo& addr){
  //connect()返回0成功。否则它返回SOCKET_ERROR
  return !::connect(sock, addr.ai_addr, static_cast<socklen_t>(addr.ai_addrlen));
}

bool SocketUtil::BindAndListen(SocketUtil::SocketFD &sock, addrinfo &addr) {
  //bind() listen()返回0成功。否则它返回SOCKET_ERROR
  return !(::bind(sock, addr.ai_addr, static_cast<socklen_t>(addr.ai_addrlen))
          || ::listen(sock, 5));
}

SocketUtil::SocketFD
SocketUtil::CreateSocket(const std::string &host, const int &port,
                         const std::function<bool(int&, addrinfo&)> &fn){
  struct addrinfo hints{};
  struct addrinfo *result;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result)) {
    return ERROR_SOCKET;
  }
  for (auto p = result; p ; p = p->ai_next) {
    auto sock =  socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == ERROR_SOCKET){
      continue;
    }
    SetOption(sock);
    // success
    if (fn(sock, *result)) {
      freeaddrinfo(result);
      return sock;
    }
    ::close(sock);
  }
  freeaddrinfo(result);
  return ERROR_SOCKET;
}

ssize_t SocketUtil::Write(SocketFD &socket_fd, const std::string &msg) {
  if (!msg.empty()){
    log(::write(socket_fd,msg.c_str(),msg.size()));
  }
  log("write finished");
  return 0;
}

std::string SocketUtil::Read(SocketFD &socket_fd) {
#ifdef DEBUG
  std::cout << "fd :" << socket_fd << std::endl;
#endif
  int size = 1024;
  char buf[size];
  ssize_t len = 0;
  std::string res;

  while ((len = (::recv(socket_fd, buf, size,0))) > 0) {
    log(len);
    res.append(buf,len);
    if (len == 127)
      break;
  }
  log(len);
  log("read finished");
  log(res);
  return res;
}
SocketUtil::SocketFD SocketUtil::Accept(SocketFD &sock, sockaddr_in &remote_addr, socklen_t &len) {
  SocketFD client_fd = ::accept(sock, reinterpret_cast<sockaddr*>(&remote_addr), &len);
  SetOption(client_fd);
  return client_fd;
}
int SocketUtil::Close(SocketUtil::SocketFD &socket_fd) {
  return ::close(socket_fd);
}

void SocketUtil::AddAfterCreateSocket(const std::function<void (const SocketFD&)> &fn) {
  SocketUtil::after_create_socket.push_back(fn);
}
void SocketUtil::SetOption(const SocketFD &sock) {
  for (const auto &afterCreateSocket : after_create_socket) {
    afterCreateSocket(sock);
  }
}
