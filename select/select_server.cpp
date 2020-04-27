//
// Created by tuffy on 2020/4/24.
//

#include "select_server.h"

#include <cstdio>
#include <fcntl.h>
#include <set>
#include <sys/select.h>
#include <utility>
SelectServer::SelectServer(std::string host, int port):
    host(std::move(host)), port(port) ,context(), running(false) {
  this->AddAfterCreateTasks([this](int socket_fd){
    this->SetNoBlocking(socket_fd);
  });
}

void SelectServer::Start() {
  int listen_fd = this->CreateServer(host,port);
  if (listen_fd == ERROR_SOCKET) {
    std::cout << "listen error" << std::endl;
  }
  running = true;
  fd_set rset, wset;
  FD_ZERO(&rset);
  FD_ZERO(&wset);
  FD_SET(listen_fd, &rset);
  int max_fd = listen_fd;
  while (running) {
    fd_set read_set = rset;
    fd_set write_set = wset;
    int n = select(max_fd + 1, &read_set, &write_set, nullptr, nullptr);
    if (n == 0) {
      std::cout << " error" << std::endl;
      continue;
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

      max_fd = std::max(max_fd, std::max(client_fd, remote_fd));
      std::cout << "max:" << max_fd << std::endl;
      FD_SET(client_fd, &rset);
      FD_SET(remote_fd, &wset);

      // todo echo
      ChannelContext client_t(client_fd, remote_fd, 1024);
      context.AddChannel(client_t);
    }
    for (const auto & i : context) {
      auto channel = i.second;
      std::cout << " client :"<< channel->client.fd << " :" << FD_ISSET(channel->client.fd, &read_set)
      << ":"<< FD_ISSET(channel->client.fd, &write_set)<< std::endl;
      std::cout << " remote :"<< channel->des.fd << " :" << FD_ISSET(channel->des.fd, &read_set)
      << ":"<< FD_ISSET(channel->client.fd, &write_set)<< std::endl;
      if (!ReadIntoChannel(channel->client, channel->des, read_set, rset, wset)) {
        remove_fd.insert(channel->client.fd);
      }
      if (!WriteFromChannel(channel->des, channel->client, write_set, rset, wset)) {
        remove_fd.insert(channel->des.fd);
      }

      if (!ReadIntoChannel(channel->des, channel->client, read_set, rset, wset)) {
        remove_fd.insert(channel->des.fd);
      }
      if (!WriteFromChannel(channel->client, channel->des, write_set, rset, wset)) {
        remove_fd.insert(channel->client.fd);
      }
    }
    for (const auto &item : remove_fd) {
      auto ptr = context.RemoveChannel(item);
      if (!ptr->client.closed){
        this->Close(ptr->client.fd);
      }
      if (!ptr->des.closed){
        this->Close(ptr->client.fd);
      }
    }
  }
}
void SelectServer::Stop() {
  running = false;
}
void SelectServer::SetNoBlocking(int socket_fd) {
  fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_SETFL, 0) | O_NONBLOCK);
}
BaseServer::SocketFD SelectServer::CreateClient(const std::string &h,
                                                const int &p) {
  using namespace std::placeholders;
  return CreateSocket(h, p, std::bind(&SelectServer::Connect,*this,_1,_2));
}
bool SelectServer::Connect(BaseServer::SocketFD sock, const addrinfo &addr) {
  int ret = ::connect(sock, addr.ai_addr, static_cast<socklen_t>(addr.ai_addrlen));
  if (ret == -1) {
    std::cout << "connect :" <<errno << std::endl;
  }
  return ret == 0 || errno == EINPROGRESS;
}

bool SelectServer::ReadIntoChannel(Channel &channel,
                                   Channel &other,
                                   fd_set &read_set,
                                   fd_set &rset,
                                   fd_set &wset) {
  if (FD_ISSET(channel.fd, &read_set) && !channel.closed) {
    if (other.buffer->IsReadable()) {
      int len = other.buffer->Read(channel.fd);
      if (len <= 0 && errno != EAGAIN) {
        printf("fd: %d read error, len:%d, error: %d\n", channel.fd,
               len, errno);
        this->Close(channel.fd);
        channel.closed = true;
        FD_CLR(channel.fd, &rset);
        FD_CLR(channel.fd, &wset);
        return false;
      } else {
        std::cout << "recv client msg\n"
                  << std::string(other.buffer->GetWriteIndex(),
                                 other.buffer->GetSize())
                  << std::endl;
        std::cout<< "notice write:" << other.fd << std::endl;
        FD_SET(other.fd, &wset);
      }
    }
  }
  return true;
}
bool SelectServer::WriteFromChannel(Channel &channel,
                                    Channel &other,
                                    fd_set &write_set,
                                    fd_set &rset,
                                    fd_set &wset) {
  if (FD_ISSET(channel.fd, &write_set)) {
    if (channel.buffer->IsWriteable()) {
      int len = channel.buffer->Write(channel.fd);
      if (len <= 0 && errno != EAGAIN) {
        printf("fd: %d wirte error, len:%d, error: %d\n", channel.fd, len , errno);
        this->Close(channel.fd);
        channel.closed = true;
        FD_CLR(channel.fd, &rset);
        FD_CLR(channel.fd, &wset);
        return false;
      }
      FD_SET(channel.fd, &rset);
      if (channel.buffer->IsEmpty()) {
        FD_CLR(channel.fd, &wset);
      }
    }
  }
  return true;
}

