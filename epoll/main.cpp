//
// Created by tuffy on 2020/5/6.
//

#include "epoll_server.h"

int main(){
  EpollServerMaster server("0.0.0.0", 8081);
  server.Start();
}