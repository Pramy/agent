//
// Created by tuffy on 2020/4/28.
//

#ifndef AGENT_SELECT_THREAD_MULTI_THREAD_SELECT_SERVER_H_
#define AGENT_SELECT_THREAD_MULTI_THREAD_SELECT_SERVER_H_
#include "select/select_server.h"

#include <thread>
#include <boost/lockfree/spsc_queue.hpp>

class ChildThread : public SelectServer {

 public:
  typedef std::function<void(ChildThread&)> Task;
  ChildThread();
  ChildThread(const ChildThread&) = default;
  virtual ~ChildThread() = default;

  void Start() override;

  bool AddTaskBeforeLoop(const Task &fn);
 private:
  std::shared_ptr<boost::lockfree::spsc_queue<Task>> tasks;
};

class MultiThreadSelectServer : public SelectServer {

 public:
  MultiThreadSelectServer(const std::string &host, int port, unsigned thread_count = 10);
  void Start() override;

  void Stop() override;
  std::vector<ChildThread>::iterator NextChild();
  bool IsTowPower(unsigned &i);
  unsigned &AdjustSize(unsigned &i);

 protected:
  std::vector<ChildThread> child;
  unsigned child_sum;
  unsigned index;
};



#endif //AGENT_SELECT_THREAD_MULTI_THREAD_SELECT_SERVER_H_
