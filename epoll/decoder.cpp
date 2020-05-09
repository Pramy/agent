//
// Created by pramy on 2020/5/7.
//

#include "decoder.h"
const std::string &Message::GetHost() const {
  return host;
}
void Message::SetHost(const std::string &host) {
  Message::host = host;
}
int Message::GetPort() const {
  return port;
}
void Message::SetPort(int port) {
  Message::port = port;
}
std::ostream &operator<<(std::ostream &os, const Message &message) {
  os << "host: " << message.host << " port: " << message.port;
  return os;
}

bool StringTextDecoder::decode(const Channel &channel, Message *result) {
  if (result == nullptr) {
    return false;
  }
  if (channel.buffer->IsWriteable()) {
    int size = 1024;
    char buff[size];
    int len = channel.buffer->Write(buff, size);
    std::string msg(buff, len);
    int pos = msg.find_last_of(':');
    std::string host = msg.substr(0, pos);
    std::string port = msg.substr(pos + 1);
    result->SetHost(host);
    result->SetPort(std::stoi(port));
    return true;
  }
  return false;
}
