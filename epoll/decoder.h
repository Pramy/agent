//
// Created by pramy on 2020/5/7.
//

#ifndef AGENT_EPOLL_DECODER_H_
#define AGENT_EPOLL_DECODER_H_

#include <boost/optional.hpp>
#include "select/context.h"

class Message {

 public:
  const std::string &GetHost() const;
  void SetHost(const std::string &host);
  int GetPort() const;
  void SetPort(int port);
 private:
  std::string host;
  int port;
};

class Decoder {

  virtual Message decode(const Channel& channel);
};

class StringTextDecoder : public  Decoder {

  boost::optional<class T>() Message decode(const Channel &channel) override;
};

#endif //AGENT_EPOLL_DECODER_H_
