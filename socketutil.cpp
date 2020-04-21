//
// Created by tuffy on 2020/4/21.
//

#include "socketutil.h"

#include <sys/socket.h>
#include <string>
#include <unistd.h>

int SocketUtil::create_socket(const std::string &host, int port, const std::function<bool(int, addrinfo&)> &fn){
    struct addrinfo hints{};
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result);
    int sock =  socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    fn(sock, *result);
    freeaddrinfo(result);
    return sock;
}

int SocketUtil::write(int &socket_fd, const std::string &msg) {
    return ::write(socket_fd,msg.c_str(),msg.size());
}

std::string SocketUtil::read(int &socket_fd) {
    int size = 1024;
    char buf[size];
    int len;
    std::string res;
    while ((len = (::read(socket_fd, buf, size))) > 0) {
        res.append(buf,len);
        if (len < size || len == 0) break;
    }
    return res;
}
