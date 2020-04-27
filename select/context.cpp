//
// Created by tuffy on 2020/4/24.
//

#include "context.h"

Channel::Channel(int fd, int size) : fd(fd), closed(false), buffer(new Buffer(size)) {}

ChannelContext::ChannelContext(int client, int des, int buffer_size) :
                  client(client, buffer_size), des(des, buffer_size){}

Channel ChannelContext::getClient() const { return client; }

Channel ChannelContext::getDes() const { return des; }

bool ChannelContext::IsSingle() {
  return client.fd == des.fd;
}

Context::Context() : client_to_des(), des_to_client() {}

void Context::AddChannel(const ChannelContext &channel) {
  auto item = std::make_shared<ChannelContext>(channel);
  client_to_des.emplace(channel.client.fd, item);
  des_to_client.emplace(channel.des.fd, item);
}

std::shared_ptr<ChannelContext> Context::GetChannel(int socket_fd) {
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
  auto item = GetChannel(socket_fd);
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
