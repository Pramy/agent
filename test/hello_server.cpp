#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>

using namespace std;

int main() {

  int server = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in addr;
  bzero(&addr, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(1234);
  if (0 != ::bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) ) {
    cout << "bind error :" << errno << endl;
  }
  std::cout <<::listen(server, 20) << std::endl;

  int ep = epoll_create(1024);
  epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = server;
  epoll_ctl(ep, EPOLL_CTL_ADD, server, &ev);


  int  size = 20;
  epoll_event events[size];
  map<int, pair<int, char*>> mpp;
  while (true) {
    int sum = epoll_wait(ep, events, size, 1000);
    for (int i = 0; i < sum; i++) {
      int fd = events[i].data.fd;
      if (server == fd) {
        sockaddr_in client_addr;
        socklen_t len = 0;
        int client = accept(server,reinterpret_cast<sockaddr *>(&client_addr), &len);
        if (client == -1 ) {
          cout <<  errno << endl;
        }
        char *str = inet_ntoa(client_addr.sin_addr);
        std::cout << "accept client new :" << str << "fd:" << client << endl;;
        ev.data.fd = client;
        ev.events = EPOLLIN| EPOLLOUT;
        epoll_ctl(ep, EPOLL_CTL_ADD, client, &ev);
        // }
      } else if (events[i].events & EPOLLIN) {
        if (mpp[fd].second == nullptr) {
          mpp[fd] = make_pair(0,new char[1024]);
        }
        mpp[fd].first = recv(fd, mpp[fd].second, 1024, 0);
        int n = mpp[fd].first;
        if (n == EAGAIN) {
          continue;
        }
        if (n == 0) {
          cout << "close :" << fd << endl;
          close(fd);
          epoll_ctl(ep, EPOLL_CTL_DEL ,fd,events+i);
          continue;
        }
        mpp[fd].second[n] = '\0';
        cout << "read :" << mpp[fd].second << endl;
        cout << fd << endl;

      } else if (events[i].events & EPOLLOUT) {
        if (mpp[fd].second == nullptr) {
          mpp[fd] = make_pair(0,new char[1024]);
        }
        if (mpp[fd].first > 0) {
          cout << "write event, msg:" << std::string(mpp[fd].second, mpp[fd].first) << endl;
          send(fd, mpp[fd].second, mpp[fd].first, 0);
          mpp[fd].first = 0;
        }
      }
    }
  }
  close(server);
}