//
// Created by tuffy on 2020/4/24.
//

#ifndef AGENT_SELECT_SERVER_H
#define AGENT_SELECT_SERVER_H

#include "base_server.h"
#include "context.h"

class SelectServer :  public BaseServer{

public:
  SelectServer(std::string host, int port);

  void Start();
//
  void Stop();

  SocketFD CreateClient(const std::string &h, const int &p) override;

  bool Connect(SocketFD sock, const addrinfo &addr) override;

  bool ReadIntoChannel(Channel &channel,
                       Channel &other,
                       fd_set &read_set ,
                       fd_set &rset,
                       fd_set &wset);

  bool WriteFromChannel(Channel &channel,
                        Channel &other,
                        fd_set &write_set ,
                        fd_set &rset,
                        fd_set &wset);

protected:

  void SetNoBlocking(int socket_fd);

  Context context;
  std::string host;
  int port;
  volatile bool running ;
};

#endif // AGENT_SELECT_SERVER_H
