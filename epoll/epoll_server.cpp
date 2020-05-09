//
// Created by tuffy on 2020/5/6.
//

#include "epoll_server.h"

#include <utility>
EpollBase::EpollBase() : context(), decoder(new StringTextDecoder()), running(false) {
  this->AddAfterCreateTasks([this](int socket_fd) {
    fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_SETFL, 0) | O_NONBLOCK);
  });
}
EpollBase::EpollBase(int size) : EpollBase() {
  this->size = size;
  this->epoll_fd = epoll_create(size);
  assert(epoll_fd > 0);
  this->events = std::shared_ptr<epoll_event>(new epoll_event[size], std::default_delete<epoll_event[]>());
}
int EpollBase::Close(BaseServer::SocketFD socket_fd) {
  auto item = context.find(socket_fd)->second;
  context.erase(item->client.fd);
  context.erase(item->des.fd);
  epoll_event event{};
  event.events = EPOLLIN | EPOLLOUT;
  event.data.fd = item->client.fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, item->client.fd, &event);
  event.data.fd = item->des.fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, item->des.fd, &event);
  item->client.closed = true;
  item->des.closed = true;
  return ::close(item->client.fd) == 0 & ::close(item->des.fd) == 0 ? 0 : -1;
}
EpollBase::~EpollBase() {
  if (epoll_fd > 0) {
    ::close(epoll_fd);
  }
}

bool EpollBase::Read(Channel &self, Channel &other) {

  if (self.fd > 0 && !self.closed && other.buffer->IsReadable()) {
    int len = other.buffer->Read(self.fd);
    if (len <= 0 && errno != EAGAIN) {
      this->Close(self.fd);
      return false;
    }
  }
  return true;
}

bool EpollBase::Write(Channel &self, Channel &other) {
  if (self.fd > 0 && !self.closed && self.buffer->IsWriteable()) {
    int len = self.buffer->Write(self.fd);
    if (len <= 0 && errno != EAGAIN) {
      this->Close(self.fd);
      return false;
    }
  }
  return true;
}
bool EpollBase::Connect(BaseServer::SocketFD sock, const addrinfo &addr) {
  int ret = ::connect(sock, addr.ai_addr, static_cast<socklen_t>(addr.ai_addrlen));
  return ret == 0 || errno == EINPROGRESS;
}
void EpollBase::SetDecoder(const std::shared_ptr<Decoder> &decoder) {
  EpollBase::decoder = decoder;
}
bool EpollBase::Connect(Channel &client, Channel &remote) {
  Message res;
  try {
    if (decoder->decode(remote, &res)) {
      SocketFD des_fd = this->CreateClient(res.GetHost(), res.GetPort());
      if (des_fd < 0) {
        std::cout << "connect error" << std::endl;
        this->Close(client.fd);
        return false;
      }
      remote.fd = des_fd;
      epoll_event event{};
      event.data.fd = des_fd;
      event.events = EPOLLIN | EPOLLOUT;
      EpollCtl(des_fd, EPOLL_CTL_ADD, &event);
      std::string msg = "ack! host: " + res.GetHost() + ", port: " + std::to_string(res.GetPort()) + "\n";
      client.buffer->Read(msg.c_str(), msg.size());
      return true;
    }
  } catch (std::invalid_argument &e) {
    std::string msg = "invalid host or port\n";
    client.buffer->Read(msg.c_str(), msg.size());
  }
  return false;
}
void EpollBase::AddChannelContext(ChannelContext *item) {
  auto ptr = shared_ptr<ChannelContext>(item);
  if (item->client.fd > 0){
    context.emplace(item->client.fd, ptr);
  }
  if (item->des.fd > 0) {
    context.emplace(item->des.fd, ptr);
  }
}
void EpollBase::EpollCtl(BaseServer::SocketFD socket_fd, int ctl_opt, epoll_event *event) {
  epoll_ctl(epoll_fd, ctl_opt, socket_fd, event);
}
const std::atomic<bool> &EpollBase::GetRunning() const {
  return running;
}

void EpollServerChild::Start() {
  bool exp = false;
  if (running.compare_exchange_strong(exp, true)) {
    while (running) {
      while (!tasks.empty()) {
        Task fn;
        if (tasks.pop(fn)) {
          fn(*this);
        }
      }
      int n = epoll_wait(epoll_fd, events.get(), size, 1000);
      for (int i = 0; i < n; ++i) {
        SocketFD socket_fd = events.get()[i].data.fd;
        auto it = context.find(socket_fd);
        if (it == context.end()) {
          continue;
        }
        auto channel_context = it->second;
        Channel &self = channel_context->client.fd == socket_fd ? channel_context->client : channel_context->des;
        Channel &other = channel_context->client.fd == self.fd ? channel_context->des : channel_context->client;
        if (events.get()[i].events & EPOLLIN) {
          EpollBase::Read(self, other);
          if (!channel_context->IsConnected() && EpollBase::Connect(channel_context->client, channel_context->des)) {
            context.emplace(channel_context->des.fd, channel_context);
          }
        }
        if (events.get()[i].events & EPOLLOUT) {
          EpollBase::Write(self, other);
        }
      }
    }
  }
}
EpollServerChild::EpollServerChild(int epoll_size, int queue_size) : EpollBase(epoll_size), tasks(queue_size) {}
bool EpollServerChild::AddTaskBeforeLoop(const EpollServerChild::Task &fn) {
  return tasks.push(fn);
}
void EpollServerChild::Stop() {
  bool exp = true;
  running.compare_exchange_strong(exp, false);
}
EpollServerMaster::EpollServerMaster(std::string host, int port, unsigned thread_count)
: EpollBase(1024), host(std::move(host)), port(port){
  thread_count = thread_count == 0 ? AdjustSize(std::thread::hardware_concurrency() * 2 ) - 1 : AdjustSize(thread_count) - 1;
  decltype(children) tmp(thread_count);
  std::cout << thread_count << std::endl;
  for (int i = 0; i < thread_count; ++i) {
    tmp[i] = std::make_shared<EpollServerChild>(1024, 1024);
  }
  children.swap(tmp);
  child_sum = thread_count;
  index = 0;
}
void EpollServerMaster::Start() {
  bool exp = false;
  if (running.compare_exchange_strong(exp, true)) {
    int listen_fd = this->CreateServer(host, port);
    if (listen_fd == ERROR_SOCKET) {
      std::cout << "create server error" << std::endl;
    }
    epoll_event event{};
    event.data.fd = listen_fd;
    event.events = EPOLLIN;
    EpollCtl(listen_fd, EPOLL_CTL_ADD, &event);
    while (running) {
      int n = epoll_wait(epoll_fd,events.get(),size, 1000);
      for (int i = 0; i < n; ++i) {
        if ((events.get()[i].events & EPOLLIN) && events.get()[i].data.fd == listen_fd) {
          sockaddr_in remote_addr{};
          socklen_t len;
          SocketFD client_fd = Accept(listen_fd, remote_addr, len);
          if (client_fd < 0 && errno != EAGAIN) {
            std::cout << "accept error" << std::endl;
          }
          auto child_shared_ptr = NextChild();
          while (!child_shared_ptr->AddTaskBeforeLoop([client_fd](EpollServerChild &child){
            child.AddChannelContext(new ChannelContext(client_fd, ERROR_SOCKET, 1024));
            epoll_event e{};
            e.data.fd = client_fd;
            e.events = EPOLLIN | EPOLLOUT;
            child.EpollCtl(client_fd, EPOLL_CTL_ADD, &e);
          }));
          if (!child_shared_ptr->GetRunning()) {
            std::thread([child_shared_ptr](){
              std::cout << std::this_thread::get_id() << std::endl;
              child_shared_ptr->Start();
            }).detach();
          }
        }
      }
    }
  }
}
void EpollServerMaster::Stop() {
  for (auto &item : children) {
    item->Stop();
  }
  bool exp = true;
  running.compare_exchange_strong(exp, false);
}
shared_ptr<EpollServerChild> EpollServerMaster::NextChild() {
  return children[index++ & child_sum];
}
