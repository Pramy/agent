//
// Created by tuffy on 2020/5/11.
//

#ifndef AGENT_EPOLL_CHANNAL_PIPE_CONTEXT_H_
#define AGENT_EPOLL_CHANNAL_PIPE_CONTEXT_H_
#include "select/context.h"
#include <unistd.h>
#include <fcntl.h>
class ChannelPipeContext : public ChannelContext{
 public:
  ChannelPipeContext(int client, int des, int buffer_size);

  int Transform(Channel &from, Channel &to) const;
  bool Close();
 private:
  int pipes[2]{};
};

#endif //AGENT_EPOLL_CHANNAL_PIPE_CONTEXT_H_
