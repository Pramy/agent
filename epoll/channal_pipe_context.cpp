//
// Created by tuffy on 2020/5/11.
//

#include "channal_pipe_context.h"

ChannelPipeContext::ChannelPipeContext(int client, int des, int buffer_size) : ChannelContext(client, buffer_size, des, buffer_size) {
  pipe(pipes);
  fcntl(pipes[0], F_SETFL, fcntl(pipes[0], F_GETFL, 0) | O_NONBLOCK);
  fcntl(pipes[1], F_SETFL, fcntl(pipes[1], F_GETFL, 0) | O_NONBLOCK);
}
int ChannelPipeContext::Transform(Channel &from, Channel &to) const {
  unsigned flag = SPLICE_F_MORE | SPLICE_F_MOVE | SPLICE_F_NONBLOCK;
  int n = splice(from.fd, nullptr, pipes[1], nullptr, 4096,flag);
  if (n <= 0) {
    return n;
  }
  n = splice(pipes[0], nullptr, to.fd, nullptr, 4096,flag);
  if (n <= 0) {
    return n;
  }
  return n;
}
bool ChannelPipeContext::Close() {
  return ::close(pipes[0]) & ::close(pipes[1]);
}
