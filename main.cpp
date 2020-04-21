//
// Created by tuffy on 2020/4/20.
//

#include <iostream>
#include <httplib.h>
//#include <Sion/Sion.h>

std::string getIP(char *hostname) {
    addrinfo hints{}, * res;
    in_addr addr{};
    int err;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET; // ipv4
    if ((err = getaddrinfo(hostname, NULL, &hints, &res)) != 0)
    {
        std::cout << "error" << std::endl;
    }
    addr.s_addr = ((sockaddr_in*)(res->ai_addr))->sin_addr.s_addr;
    char str[16];
    inet_ntop(AF_INET, &addr, str, sizeof(str));
    freeaddrinfo(res);
    return std::string(str);
}

int main(){

}