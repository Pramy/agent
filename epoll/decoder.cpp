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
boost::optional<message> StringTextDecoder::decode(const Channel &channel) {
  boost::optional<Message> op;
  op.

}
