#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

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


  int size = 20;
  epoll_event events[size];
  while (true) {
    int sum = epoll_wait(ep, events, size, 1000);
    for (int i = 0; i < sum; i++) {
      if (server == events[i].data.fd) {
        sockaddr_in client_addr;
        socklen_t len = 0;
        int client = accept(server,reinterpret_cast<sockaddr *>(&client_addr), &len);
        if (client == -1 ) {
          cout <<  errno << endl;
        } 
        char *str = inet_ntoa(client_addr.sin_addr);
        std::cout << "accept client new :" << str << "fd:" << client << endl;;
        ev.data.fd = client;
        ev.events = EPOLLIN;
        epoll_ctl(ep, EPOLL_CTL_ADD, client, &ev);
        // }
      } else if (events[i].events & EPOLLIN) {
        int n, size = 1024;
        char buff[size];
        n = recv(events[i].data.fd, buff, size, 0);
        if (n == EAGAIN) {
          continue;
        }
        if (n == 0) {
          cout << "close :" << events[i].data.fd << endl;
          close(events[i].data.fd);
          continue;
        }
        buff[n] = '\0';
        cout << "read :" << buff << endl;
        cout << events[i].data.fd << endl;

        std::string msg = "hello world\n";
        cout << msg << endl;
        send(events[i].data.fd, msg.c_str(), msg.size(), 0);
      }
    }
  }
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
//       if (events[i].data.fd == listenfd) {
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
//       //   if ((sockfd = events[i].data.fd) < 0)
//       //     continue;
//       //   if ((n = recv(sockfd, line, sizeof(line), 0)) < 0) {
//       //     // Connection
//       //     //
//       //     Reset:你连接的那一端已经断开了，而你却还试着在对方已断开的socketfd上读写数据！
//       //     if (errno == ECONNRESET) {
//       //       close(sockfd);
//       //       events[i].data.fd = -1;
//       //     } else
//       //       std::cout << "readline error" << std::endl;
//       //   } else if (n == 0) //读入的数据为空
//       //   {
//       //     close(sockfd);
//       //     events[i].data.fd = -1;
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
//       //   sockfd = events[i].data.fd;
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