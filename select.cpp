//
// Created by tuffy on 2020/4/23.
//
#include "select/select_server.h"

int main()
{
  SelectServer server("0.0.0.0", 8081);
  server.Start();
}