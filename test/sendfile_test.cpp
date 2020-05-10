//
// Created by pramy on 2020/5/10.
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

#define LISTENQ 100


int main(int argc,char** argv) {
  if (argc != 3) {
    printf("please add <port> <sendfile-name>\n");
    return -1;
  }
  int sockfd;
  if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    printf("socket error: %s\n",strerror(errno));
    return -1;
  }
  struct sockaddr_in server;
  bzero(&server,sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(atoi(argv[1]));
  if (bind(sockfd,(struct sockaddr*)&server,sizeof(server)) < 0) {
    printf("bind error: %s\n",strerror(errno));
    return -1;
  }
  if (listen(sockfd,LISTENQ) < 0) {
    printf("listen error: %s\n",strerror(errno));
    return -1;
  }
  int connfd;
  for (; ;) {
    if ((connfd = accept(sockfd,NULL,NULL)) < 0) {
      if (errno == EINTR) {
        continue;
      }
      printf("accept error: %s\n",strerror(errno));
      return -1;
    }
    struct stat file_stat;
    bool work = true;
    if (stat(argv[2],&file_stat) < 0) {
      printf("stat error: %s\n",strerror(errno));
      work = false;
    }
    if (S_ISREG(file_stat.st_mode)) {
      int fd;
      if ((fd = open(argv[2],O_RDONLY)) < 0) {
        printf("open error: %s\n",strerror(errno));
        return -1;
      }
      if (sendfile(connfd,fd,nullptr,file_stat.st_size) != file_stat.st_size) {
        work = false;
        printf("sendfile error: %s\n",strerror(errno));
        return -1;
      }
      work = false;
      close(connfd);
      close(fd);
      continue;
    }
    if (!work) {
      char buf[100] = "Invalid request\n";
      if (write(connfd,buf,strlen(buf)) != strlen(buf)) {
        close(connfd);
        printf("write error: %s\n",strerror(errno));
        continue;
      }
      close(connfd);
    }
  }
  return 0;
}