//
// Created by tuffy on 2020/4/28.
//

#include "multi_thread_select_server.h"

void MultiThreadSelectServer::Start() {
  bool exp = false;
  if (running.compare_exchange_strong(exp, true)){
    int listen_fd = this->CreateServer(host,port);
    if (listen_fd == ERROR_SOCKET) {
      std::cout << "listen error" << std::endl;
    }
    max_fd = listen_fd;
    FD_SET(listen_fd, &rset);
    timeval timeval{};
    timeval.tv_sec = 1;
    timeval.tv_usec = 0;
    while (running) {
      fd_set read_set = rset;
      fd_set write_set = wset;
      int n = select(max_fd + 1, &read_set, &write_set, nullptr, &timeval);
      if (n <= 0) {
        if (n == 0){
          continue;
        }
        printf("select error, n = %d, errno = %d", n, errno);
        break;
      }
      std::set<int> remove_fd;
      if (FD_ISSET(listen_fd, &read_set)) {
        sockaddr_in remote_addr{};
        socklen_t len;
        int client_fd = this->Accept(listen_fd, remote_addr, len);
        if (client_fd < 0 && errno != EAGAIN) {
          std::cout << "accept error" << std::endl;
        }

        int remote_fd = this->CreateClient("www.baidu.com", 80);
        if (remote_fd == ERROR_SOCKET) {
          std::cout << "connect error" << std::endl;
          this->Close(client_fd);
        }
        auto child_it = NextChild();
        auto fn = [client_fd, remote_fd](ChildThread& child_thread){
          std::cout << "call" << std::endl;
          const ChannelContext client_t(client_fd, remote_fd, 1024);
          child_thread.GetContext().AddChannel(client_t);
          auto &rset = child_thread.GetRset();
          auto &wset = child_thread.GetRset();
          FD_SET(client_fd, &const_cast<fd_set&>(rset));
          FD_SET(remote_fd, &const_cast<fd_set&>(wset));
          child_thread.SetMaxFd(std::max(child_thread.GetMaxFd(), std::max(client_fd, remote_fd)));
        };
        while (!child_it->AddTaskBeforeLoop(fn));
        bool expc = false;
        auto & running = const_cast<std::atomic<bool>&>(child_it->GetRunning());
        if (running.compare_exchange_strong(expc, true)) {
          std::thread([child_it](){
            child_it->Start();
          }).detach();
        }
        // todo echo
      }
    }
  } else {
    std::cout << "master server already start"<< std::endl;
  }

}
MultiThreadSelectServer::MultiThreadSelectServer(const std::string &host, int port, unsigned thread_count)
  : SelectServer(host, port), child(thread_count == 0? std::thread::hardware_concurrency() * 2 + 1 : AdjustSize(thread_count), ChildThread()),
  child_sum(thread_count == 0? std::thread::hardware_concurrency() * 2 + 1 : thread_count), index(0){
}
void MultiThreadSelectServer::Stop() {
  for (auto &item : child) {
    item.Stop();
  }
}

inline
std::vector<ChildThread>::iterator MultiThreadSelectServer::NextChild() {
  return child.begin() + (index++ & child_sum - 1);
}

inline
bool MultiThreadSelectServer::IsTowPower(unsigned &i) {
  return (i & -i) == i;
}

inline
unsigned &MultiThreadSelectServer::AdjustSize(unsigned &i) {
  if (!IsTowPower(i)) {
    i--;
    i |= i >> 1u;
    i |= i >> 2u;
    i |= i >> 4u;
    i |= i >> 8u;
    i |= i >> 16u;
  }
  return i;
}

ChildThread::ChildThread():SelectServer(),tasks(new boost::lockfree::spsc_queue<Task>(1024)){}

void ChildThread::Start() {
  std::cout << "child start" << std::endl;
  if (running){
    timeval timeval{};
    timeval.tv_sec = 1;
    timeval.tv_usec = 1;
    while (running) {
      while (!tasks->empty()) {
        Task fn;
        if (tasks->pop(fn)){
          fn(*this);
        }
      }
      fd_set read_set = rset;
      fd_set write_set = wset;

      int n = select(max_fd + 1, &read_set, &write_set, nullptr, &timeval);
      if (n == 0) {
        continue;
      }
      std::set<int> remove_fd;
      for (const auto & i : context) {
        auto channel = i.second;
        std::cout << " client :"<< channel->client.fd << " :" << FD_ISSET(channel->client.fd, &read_set)
                  << ":"<< FD_ISSET(channel->client.fd, &write_set)<< std::endl;
        std::cout << " remote :"<< channel->des.fd << " :" << FD_ISSET(channel->des.fd, &read_set)
                  << ":"<< FD_ISSET(channel->client.fd, &write_set)<< std::endl;
        if (!ReadIntoChannel(channel->client, channel->des, read_set)) {
          remove_fd.insert(channel->client.fd);
        }
        if (!WriteFromChannel(channel->des, channel->client, write_set)) {
          remove_fd.insert(channel->des.fd);
        }

        if (!ReadIntoChannel(channel->des, channel->client, read_set)) {
          remove_fd.insert(channel->des.fd);
        }
        if (!WriteFromChannel(channel->client, channel->des, write_set)) {
          remove_fd.insert(channel->client.fd);
        }
      }
      for (const auto &item : remove_fd) {
        std::cout << "remove:" << item << std::endl;
        auto ptr = context.RemoveChannel(item);
        if (!ptr->client.closed){
          this->Close(ptr->client.fd);
        }
        if (!ptr->des.closed){
          this->Close(ptr->client.fd);
        }
      }
    }
    this->CloseAll();
  }
}
bool ChildThread::AddTaskBeforeLoop(const std::function<void(ChildThread &)> &fn) {
  return tasks->push(fn);
}

