//
// Created by tuffy on 2020/4/23.
//

//
// Created by tuffy on 2020/4/21.
//
#include "base_server.h"

#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

template <class T>
inline
void RunTasks(const std::vector<T> &tasks, BaseServer::SocketFD sock){
  for (const auto &item : tasks) {
    item(sock);
  }
}


template <class T>
inline
void RunTasks(const std::vector<T> &tasks,
              BaseServer::SocketFD sock,
              const std::string &msg){
  for (const auto &item : tasks) {
    item(sock, msg);
  }
}

BaseServer::SocketFD
BaseServer::CreateServer(const std::string &host, const int &port){
  using namespace std::placeholders;
  return BaseServer::CreateSocket(host, port, std::bind(&BaseServer::BindAndListen,*this,_1,_2));
}

BaseServer::SocketFD
BaseServer::CreateClient(const std::string &host, const int &port){
  using namespace std::placeholders;
  return CreateSocket(host, port, std::bind(&BaseServer::Connect,*this,_1,_2));
}

bool BaseServer::Connect(SocketFD sock, const addrinfo& addr){
  //connect()返回0成功。否则它返回SOCKET_ERROR
  return !::connect(sock, addr.ai_addr, static_cast<socklen_t>(addr.ai_addrlen));
}

bool BaseServer::BindAndListen(SocketFD sock, const addrinfo &addr) {
  int val = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val)) < 0) {
      return false;
  }
  //bind() listen()返回0成功。否则它返回SOCKET_ERROR
  return !(::bind(sock, addr.ai_addr, static_cast<socklen_t>(addr.ai_addrlen))
           || ::listen(sock, 5));
}

BaseServer::SocketFD
BaseServer::CreateSocket(const std::string &host,
                         const int &port,
                         const BaseServer::SocketCreateCallFun &fn){
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
    SocketFD sock =  socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == ERROR_SOCKET){
      continue;
    }
    RunTasks(after_create_tasks, sock);
    if (fn(sock, *result)) {
      freeaddrinfo(result);
      return sock;
    }
    ::close(sock);
  }
  freeaddrinfo(result);
  return ERROR_SOCKET;
}

ssize_t BaseServer::Write(SocketFD socket_fd, const std::string &msg) {
  RunTasks(before_write_tasks, socket_fd, msg);
  if (!msg.empty()){
    ::send(socket_fd, msg.c_str(), msg.size(), 0);
  }
  RunTasks(after_write_tasks, socket_fd, msg);
  return 0;
}

std::string BaseServer::Read(SocketFD socket_fd) {
  RunTasks(before_read_tasks, socket_fd);
  char buf[MAX_BUF_SIZE];
  ssize_t len = 0;
  std::string res;

  while ((len = (::recv(socket_fd, buf, MAX_BUF_SIZE,0))) > 0) {
    res.append(buf,len);
  }
  RunTasks(after_read_tasks, socket_fd, res);
  return res;
}
BaseServer::SocketFD BaseServer::Accept(SocketFD sock, sockaddr_in &remote_addr, socklen_t &len) {
  SocketFD client_fd = ::accept(sock, reinterpret_cast<sockaddr*>(&remote_addr), &len);
  if (client_fd > 0) {
    RunTasks(after_accept_tasks, client_fd);
  }
  return client_fd;
}

int BaseServer::Close(SocketFD socket_fd) {
  return ::close(socket_fd);
}

void BaseServer::AddAfterCreateTasks(const SocketCallFun &fn) {
  after_create_tasks.push_back(fn);
}

void BaseServer::AddAfterAcceptTasks(const SocketCallFun &fn) {
  after_accept_tasks.push_back(fn);
}

void BaseServer::AddAfterConnectTasks(const SocketCallFun &fn) {
  after_connect_tasks.push_back(fn);
}

void BaseServer::AddBeforeReadTasks(const BaseServer::SocketCallFun &fn) {
  before_read_tasks.push_back(fn);
}

void BaseServer::AddAfterReadTasks(const BaseServer::SocketMsgFun &fn) {
  after_read_tasks.push_back(fn);
}

void BaseServer::AddBeforeWriteTasks(const BaseServer::SocketMsgFun &fn) {
  before_write_tasks.push_back(fn);
}

void BaseServer::AddAfterWriteTasks(const BaseServer::SocketMsgFun &fn) {
  after_write_tasks.push_back(fn);
}
