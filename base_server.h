//
// Created by tuffy on 2020/4/23.
//

#ifndef AGENT_BASE_SERVER_H
#define AGENT_BASE_SERVER_H
//#define PRINT(a) (std::cout << a << std::endl)
#define ERROR_SOCKET (-1)

#include <netdb.h>

#include <string>
#include <vector>
#include <iostream>
#include <functional>

class BaseServer{

public:

  typedef int SocketFD;

  typedef std::function<void(SocketFD)> SocketCallFun;

  typedef std::function<bool(SocketFD, const addrinfo&)> SocketCreateCallFun;

  typedef std::function<void(SocketFD, const std::string&)> SocketMsgFun;

  BaseServer(): after_create_tasks(), after_accept_tasks(), after_connect_tasks(),
                before_read_tasks(), after_read_tasks(), before_write_tasks(),
                after_write_tasks() {};

  virtual SocketFD CreateServer(const std::string &host, const int &port);

  virtual SocketFD CreateClient(const std::string &host, const int &port);

  virtual SocketFD Accept(SocketFD sock,sockaddr_in &remote_addr, socklen_t &len);

  virtual ssize_t Write(SocketFD socket_fd, const std::string &msg);

  virtual std::string Read(SocketFD socket_fd);

  virtual int Close(SocketFD socket_fd);

  void AddAfterCreateTasks(const SocketCallFun &fn);

  void AddAfterAcceptTasks(const SocketCallFun &fn);

  void AddAfterConnectTasks(const SocketCallFun &fn);

  void AddBeforeReadTasks(const SocketCallFun &fn);

  void AddAfterReadTasks(const SocketMsgFun &fn);

  void AddBeforeWriteTasks(const SocketMsgFun &fn);

  void AddAfterWriteTasks(const SocketMsgFun &fn);

  template <typename T, typename... Args>
  void RunAllTasks(const std::vector<T>& tasks, Args... args);

  virtual bool Connect(SocketFD sock, const addrinfo &addr);

  virtual bool BindAndListen(SocketFD sock, const addrinfo &addr);

  virtual SocketFD CreateSocket(const std::string &host,
                                const int &port,
                                const SocketCreateCallFun &fn);

private:

   std::vector<SocketCallFun> after_create_tasks{};

   std::vector<SocketCallFun> after_accept_tasks{};

   std::vector<SocketCallFun> after_connect_tasks{};

   std::vector<SocketCallFun> before_read_tasks{};

   std::vector<SocketMsgFun> after_read_tasks{};

   std::vector<SocketMsgFun> before_write_tasks{};

   std::vector<SocketMsgFun> after_write_tasks{};
};

#endif // AGENT_BASE_SERVER_H
