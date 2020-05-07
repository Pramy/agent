//
// Created by tuffy on 2020/4/24.
//

#ifndef AGENT_SELECT_SERVER_H
#define AGENT_SELECT_SERVER_H

#include "base_server.h"

#include <sys/select.h>
#include <fcntl.h>

#include <set>
#include <atomic>
#include "context.h"


class SelectServer :  public BaseServer{

public:
  SelectServer(std::string host, int port);
  SelectServer();
  SelectServer(const SelectServer &);

  virtual void Start();
  virtual void Stop();
  void CloseAll();
  bool Connect(SocketFD sock, const addrinfo &addr) override;
  bool ReadIntoChannel(Channel &channel,
                       Channel &other,
                       fd_set &read_set);
  bool WriteFromChannel(Channel &channel,
                        Channel &other,
                        fd_set &write_set);
  void CASSetRunning(bool res);
  void CloseChannel(Channel &channel, Channel&other);

  Context &GetContext();
  const std::atomic<bool> &GetRunning() const;
  const fd_set &GetRset() const;
  const fd_set &GetWset() const;
  int GetMaxFd() const;
  const std::string &GetHost() const;
  int GetPort() const;

  void SetMaxFd(int i);

 protected:
  void SetNoBlocking(int socket_fd);
  fd_set rset;
  fd_set wset;
  int max_fd;
  Context context;
  std::string host;
  int port;
  std::atomic<bool> running;
};

#endif // AGENT_SELECT_SERVER_H
