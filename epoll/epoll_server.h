//
// Created by tuffy on 2020/5/6.
//

#ifndef AGENT_EPOLL_EPOLL_SERVER_H_
#define AGENT_EPOLL_EPOLL_SERVER_H_
#include <sys/epoll.h>
#include <fcntl.h>

#include <thread>
#include <boost/lockfree/spsc_queue.hpp>
#include <memory>

#include "base_server.h"
#include "select/context.h"


class EpollBase : public BaseServer {
 public:
  EpollBase();
  EpollBase(const std::string &host, int port, int size = 256);
  virtual ~EpollBase();

  int Close(SocketFD socket_fd) override;
  bool Connect(SocketFD sock, const addrinfo &addr) override;
  bool Read(SocketFD socket_fd);
  bool Write(SocketFD socket_fd);

  virtual void Start();
  virtual void Stop();

 protected:
  Context context;
  std::string host;
  int port;

  int epoll_fd;

  //array
  std::shared_ptr<epoll_event> events;
  int size;

  std::atomic<bool> running;
};

class EpollServerChild : public EpollBase {

};


class EpollServerMaster : public EpollBase {

};

#endif //AGENT_EPOLL_EPOLL_SERVER_H_
