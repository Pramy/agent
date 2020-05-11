//
// Created by tuffy on 2020/4/24.
//

#ifndef AGENT_CONTEXT_H
#define AGENT_CONTEXT_H

#include <map>
#include <memory>

#include "buffer.h"

class Channel {

public:
  explicit Channel(int fd, int size);
  int fd;
  std::shared_ptr<Buffer> buffer;

};

class ChannelContext {

public:
  ChannelContext(const ChannelContext &) = default;

  ChannelContext(int client, int client_size, int des, int des_size): client(client, client_size), des(des, des_size) {}

  ChannelContext(int client, int des, int buffer_size):ChannelContext(client, buffer_size, des, buffer_size){ };

  bool IsConnected() const ;
  Channel client;
  Channel des;
};

class Context{

public:
  typedef std::map<int, std::shared_ptr<ChannelContext>> ChannelMap;

  Context();

  void AddChannelContext(const ChannelContext &channel);

  std::shared_ptr<ChannelContext> GetChannelContext(int socket_fd);

  std::shared_ptr<ChannelContext> RemoveChannel(int socket_fd);

  ChannelMap::const_iterator begin() const;

  ChannelMap::const_iterator end() const;

  ChannelMap client_to_des;
  ChannelMap des_to_client;
};

#endif // AGENT_CONTEXT_H
