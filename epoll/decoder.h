//
// Created by pramy on 2020/5/7.
//

#ifndef AGENT_EPOLL_DECODER_H_
#define AGENT_EPOLL_DECODER_H_

#include <ostream>
#include "select/context.h"

class Message {

 public:
  const std::string &GetHost() const;
  void SetHost(const std::string &host);
  int GetPort() const;
  void SetPort(int port);
  friend std::ostream &operator<<(std::ostream &os, const Message &message);
 private:
  std::string host;
  int port;
};

class Decoder {

 public:
  virtual bool decode(const Channel& channel, Message *result)  = 0;
};

class StringTextDecoder : public  Decoder {

 public:
  bool decode(const Channel &channel, Message *result) override;
};

#endif //AGENT_EPOLL_DECODER_H_
