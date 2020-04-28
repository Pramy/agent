//
// Created by tuffy on 2020/4/30.
//
#include "select/thread/multi_thread_select_server.h"

int main (){
  MultiThreadSelectServer server("0.0.0.0", 8081);
  server.Start();
}
