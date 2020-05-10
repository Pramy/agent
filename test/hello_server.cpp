#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>
#include <sys/sendfile.h>
#include <fcntl.h>

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
//        int file  = open("/home/tuffychen/1.txt",O_RDWR);
//        sendfile(fd,fd, nullptr, 1024);
//        mpp[fd].first = recv(fd, mpp[fd].second, 1024, 0);
//        int n = mpp[fd].first;
//        int n = sendfile(fd,file, nullptr, 1024);
//        cout << "hello" <<file <<endl;
//        int n = sendfile(fd,file, nullptr, 1024);
        int pipefd[2];
        pipe(pipefd);
        splice(fd, nullptr, pipefd[1], nullptr, 1024,SPLICE_F_MORE);
        int n = splice(pipefd[0], nullptr, fd, nullptr, 1024,SPLICE_F_MORE);
//        splice(pipefd[0], NULL, connfd[1], NULL, 4096,SPLICE_F_MORE);
//        int n = splice(file,nullptr,fd, nullptr, 1024, SPLICE_F_NONBLOCK);
        if (n == -1) {
          if (errno == EAGAIN) {
            continue;
          }
          cout << errno <<endl;
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
        cout << "close :" << fd << endl;
//        close(fd);
        epoll_ctl(ep, EPOLL_CTL_DEL ,fd,events+i);
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
//
// Created by pramy on 2020/5/4.
//
// #include <arpa/inet.h>
// #include <boost/atomic.hpp>
// #include <cerrno>
// #include <cstdio>
// #include <cstring>
// #include <fcntl.h>
// #include <iostream>
// #include <netinet/in.h>
// #include <sys/epoll.h>
// #include <sys/socket.h>
// #include <unistd.h>
// using namespace std;

// #define MAXLINE 100
// #define OPEN_MAX 100
// #define LISTENQ 20
// #define SERV_PORT 5000
// #define INFTIM 1000

// void setnonblocking(int sock) {
//   int opts;
//   opts = fcntl(sock, F_GETFL);
//   if (opts < 0) {
//     perror("fcntl(sock,GETFL)");
//     exit(1);
//   }
//   opts = opts | O_NONBLOCK;
//   if (fcntl(sock, F_SETFL, opts) < 0) {
//     perror("fcntl(sock,SETFL,opts)");
//     exit(1);
//   }
// }

// int main(int argc, char *argv[]) {
//   int i, maxi, listenfd, connfd, sockfd, epfd, nfds, portnumber = 1234;
//   ssize_t n;
//   char line[MAXLINE];
//   socklen_t clilen;
//   string szTemp("");

//   //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
//   struct epoll_event ev, events[20];

//   //创建一个epoll的句柄，size用来告诉内核这个监听的数目一共有多大
//   epfd = epoll_create(256); //生成用于处理accept的epoll专用的文件描述符

//   struct sockaddr_in clientaddr;
//   struct sockaddr_in serveraddr;
//   listenfd = socket(AF_INET, SOCK_STREAM, 0);

//   //把socket设置为非阻塞方式
//   // setnonblocking(listenfd);

//   //设置与要处理的事件相关的文件描述符
//   ev.data.fd = listenfd;

//   //设置要处理的事件类型
//   ev.events = EPOLLIN | EPOLLET;

//   //注册epoll事件
//   epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

//   bzero(&serveraddr, sizeof(serveraddr)); /*配置Server socket的相关信息 */
//   serveraddr.sin_family = AF_INET;
//   char *local_addr = "0.0.0.0";
//   inet_aton(local_addr, &(serveraddr.sin_addr)); // htons(portnumber);
//   serveraddr.sin_port = htons(portnumber);
//   bind(listenfd, (sockaddr *)&serveraddr, sizeof(serveraddr));

//   listen(listenfd, LISTENQ);

//   maxi = 0;

//   for (;;) {

//     //等待epoll事件的发生
//     //返回需要处理的事件数目nfds，如返回0表示已超时。
//     nfds = epoll_wait(epfd, events, 20, 500);

//     //处理所发生的所有事件
//     for (i = 0; i < nfds; ++i) {
//       //如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
//       if (fd == listenfd) {
//         connfd = accept(listenfd, (sockaddr *)&clientaddr, &clilen);
//         if (connfd < 0) {
//           perror("connfd < 0");
//           exit(1);
//         }
//         // setnonblocking(connfd);
//         char *str = inet_ntoa(clientaddr.sin_addr);
//         cout << "accapt a connection from " << str << endl;

//         //设置用于读操作的文件描述符
//         ev.data.fd = connfd;

//         //设置用于注册的读操作事件
//         ev.events = EPOLLIN | EPOLLET;

//         //注册ev
//         epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev); /* 添加 */
//       }
//       //如果是已经连接的用户，并且收到数据，那么进行读入。
//       // else if (events[i].events & EPOLLIN) {
//       //   cout << "EPOLLIN" << endl;
//       //   if ((sockfd = fd) < 0)
//       //     continue;
//       //   if ((n = recv(sockfd, line, sizeof(line), 0)) < 0) {
//       //     // Connection
//       //     //
//       //     Reset:你连接的那一端已经断开了，而你却还试着在对方已断开的socketfd上读写数据！
//       //     if (errno == ECONNRESET) {
//       //       close(sockfd);
//       //       fd = -1;
//       //     } else
//       //       std::cout << "readline error" << std::endl;
//       //   } else if (n == 0) //读入的数据为空
//       //   {
//       //     close(sockfd);
//       //     fd = -1;
//       //   }
//       //   cout << "errno :" << errno << ",n :" << n << endl;
//       //   szTemp = "";
//       //   szTemp += line;
//       //   szTemp = szTemp.substr(0, szTemp.find('\r')); /* remove the enter key
//       //   */ memset(line, 0, 100);                         /* clear the buffer
//       //   */
//       //   // line[n] = '\0';
//       //   cout << "Readin: " << szTemp << endl;

//       //   //设置用于写操作的文件描述符
//       //   ev.data.fd = sockfd;

//       //   //设置用于注册的写操作事件
//       //   ev.events = EPOLLOUT | EPOLLET;

//       //   //修改sockfd上要处理的事件为EPOLLOUT
//       //   epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev); /* 修改 */

//       // } else if (events[i].events & EPOLLOUT) // 如果有数据发送

//       // {
//       //   sockfd = fd;
//       //   szTemp = "Server:" + szTemp + "\n";
//       //   send(sockfd, szTemp.c_str(), szTemp.size(), 0);

//       //   //设置用于读操作的文件描述符
//       //   ev.data.fd = sockfd;

//       //   //设置用于注册的读操作事件
//       //   ev.events = EPOLLIN | EPOLLET;

//       //   //修改sockfd上要处理的事件为EPOLIN
//       //   epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev); /* 修改 */
//       // }
//     } //(over)处理所发生的所有事件
//   }   //(over)等待epoll事件的发生

//   close(epfd);
//   return 0;
// }