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
  bool closed;
  std::shared_ptr<Buffer> buffer;

};

class ChannelContext {

public:
  ChannelContext(int client, int des, int buffer_size);
  ChannelContext(const ChannelContext &) = default;

  Channel getClient() const;
  Channel getDes() const;

  bool IsConnected();
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
