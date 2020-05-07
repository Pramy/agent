//
// Created by tuffy on 2020/5/6.
//

#include "epoll_server.h"
EpollBase::EpollBase() : context(), running(false) {
  this->AddAfterCreateTasks([this](int socket_fd) {
    fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_SETFL, 0) | O_NONBLOCK);
  });
}
EpollBase::EpollBase(const std::string &host, int port, int size) : EpollBase() {
  this->host = host;
  this->port = port;
  this->size = size;
  this->epoll_fd = epoll_create(size);
  assert(epoll_fd > 0);
  this->events = std::shared_ptr<epoll_event>(new epoll_event[size], std::default_delete<epoll_event[]>());
}
int EpollBase::Close(BaseServer::SocketFD socket_fd) {
  auto item = context.RemoveChannel(socket_fd);
  if (item) {
    epoll_event event{};
    event.events = EPOLLIN | EPOLLOUT;
    event.data.fd = item->client.fd;
    epoll_ctl(epoll_fd,EPOLL_CTL_DEL, item->client.fd,&event);
    event.data.fd = item->des.fd;
    epoll_ctl(epoll_fd,EPOLL_CTL_DEL, item->des.fd,&event);
    item->client.closed = true;
    item->des.closed = true;
    return ::close(item->client.fd) == 0 & ::close(item->des.fd) == 0 ? 0 : -1 ;
  }
  return -1;
}
EpollBase::~EpollBase() {
  if (epoll_fd > 0) {
    ::close(epoll_fd);
  }
}

bool EpollBase::Read(BaseServer::SocketFD socket_fd) {
  auto item = context.GetChannel(socket_fd);
  if (item == nullptr) {
    return false;
  }

  Channel &self = item->client.fd == socket_fd ? item->client : item->des;
  Channel &other = item->client.fd == socket_fd ? item->client : item->des;
  if (self.fd > 0 && !self.closed && other.buffer->IsReadable()){
    int len = other.buffer->Read(self.fd);
    if (len <=0 && errno != EAGAIN) {
      this->Close(socket_fd);
      return false;
    }
    if (!item->IsConnected()) {

      SocketFD des_fd = this->CreateClient();
    }
  }
  return true;
}

bool EpollBase::Write(BaseServer::SocketFD socket_fd) {
  return true;
}
bool EpollBase::Connect(BaseServer::SocketFD sock, const addrinfo &addr) {
  int ret = ::connect(sock, addr.ai_addr, static_cast<socklen_t>(addr.ai_addrlen));
  if (ret == -1) {
    std::cout << "connect :" <<errno << std::endl;
  }
  return ret == 0 || errno == EINPROGRESS;
}

