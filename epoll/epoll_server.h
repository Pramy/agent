//
// Created by tuffy on 2020/5/6.
//

#ifndef AGENT_EPOLL_EPOLL_SERVER_H_
#define AGENT_EPOLL_EPOLL_SERVER_H_
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include <thread>
#include <boost/lockfree/spsc_queue.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

#include "base_server.h"
#include "select/context.h"
#include "epoll/decoder.h"

using std::shared_ptr;
using std::vector;
using std::string;
using std::unordered_map;
class EpollBase : public BaseServer {
 public:
  EpollBase();
  explicit EpollBase(int size);
  virtual ~EpollBase();

  int Close(SocketFD socket_fd) override;
  bool Connect(SocketFD sock, const addrinfo &addr) override;
  bool Read(Channel &self, Channel &other);
  bool Write(Channel &self, Channel &other);
  bool Connect(Channel &client, Channel &remote);
  virtual void Start() = 0;
  virtual void Stop() = 0;
  void AddChannelContext(ChannelContext *item);
  void EpollCtl(SocketFD socket_fd, int ctl_opt, epoll_event *event);

  void SetDecoder(const std::shared_ptr<Decoder> &decoder);
  const std::atomic<bool> &GetRunning() const;
 protected:
  unordered_map<SocketFD, shared_ptr<ChannelContext>> context;
  std::shared_ptr<Decoder> decoder;
 protected:
  int epoll_fd;
  //array
  std::shared_ptr<epoll_event> events;
  int size;

  std::atomic<bool> running;
};

class EpollServerChild : public EpollBase {

 public:
  typedef std::function<void(EpollServerChild &)> Task;
  EpollServerChild(int epoll_size, int queue_size);

  void Start() override;
  void Stop() override;
  bool AddTaskBeforeLoop(const Task &fn);
 private:
  boost::lockfree::spsc_queue<Task> tasks;
};

class EpollServerMaster : public EpollBase {
 public:
  EpollServerMaster(std::string host, int port, unsigned thread_count = 0);
  void Start() override;
  void Stop() override;
  shared_ptr<EpollServerChild> NextChild();
 protected:
  vector<shared_ptr<EpollServerChild>> children;
  unsigned child_sum;
  unsigned index;
  string host;
  int port;
};

#endif //AGENT_EPOLL_EPOLL_SERVER_H_
