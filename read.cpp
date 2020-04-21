//
// Created by tuffy on 2020/4/21.
//

#include "socketutil.h"
#include <iostream>

#define MAXLEN  1024
int main()
{
    int client_socket = SocketUtil::create_socket("127.0.0.1", 8081, [](int sock, addrinfo &result) -> bool {
        int ret = ::connect(sock, result.ai_addr, static_cast<socklen_t>(result.ai_addrlen));
        return true;
    });
    SocketUtil::write(client_socket, "hello world");
    std::cout << SocketUtil::read(client_socket) << std::endl;
}
