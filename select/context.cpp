//
// Created by tuffy on 2020/4/24.
//

#include "context.h"

Channel::Channel(int fd, int size) : fd(fd), buffer(new Buffer(size)) {}

bool ChannelContext::IsConnected() const {
  return des.fd > 0 ;
}

Context::Context() : client_to_des(), des_to_client() {}

void Context::AddChannelContext(const ChannelContext &channel) {
  auto item = std::make_shared<ChannelContext>(channel);
  if (item->client.fd > 0) {
    client_to_des.emplace(channel.client.fd, item);
  }
  if (item->des.fd > 0) {
    des_to_client.emplace(channel.des.fd, item);
  }
}

std::shared_ptr<ChannelContext> Context::GetChannelContext(int socket_fd) {
  auto res = client_to_des.find(socket_fd);
  if (res != client_to_des.end()){
    return res->second;
  }
  res = des_to_client.find(socket_fd);
  if (res != des_to_client.end()){
    return res->second;
  }
  return nullptr;
}
std::shared_ptr<ChannelContext> Context::RemoveChannel(int socket_fd) {
  auto item = GetChannelContext(socket_fd);
  if(nullptr == item){
    return nullptr;
  }
  client_to_des.erase(item->client.fd);
  des_to_client.erase(item->des.fd);
  return item;
}
Context::ChannelMap::const_iterator Context::begin() const{
  return client_to_des.cbegin();
}
Context::ChannelMap::const_iterator Context::end() const{
  return client_to_des.cend();
}
