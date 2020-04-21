//
// Created by tuffy on 2020/4/21.
//

#ifndef AGENT_SOCKETUTIL_H
#define AGENT_SOCKETUTIL_H


#include <netdb.h>
#include <string>

class SocketUtil{
public:
    static int create_socket(const std::string &host, int port, const std::function<bool(int, addrinfo&)> &fn);

    static int write(int &socket_fd, const std::string &msg);

    static std::string read(int &socket_fd);
};



#endif //AGENT_SOCKETUTIL_H
