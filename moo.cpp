//
// Created by tuffy on 2020/4/21.
//

#include <iostream>
#include <httplib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void) {
    std::string host = "www.baidu.com";
//    httplib::Client cli(host, 8081);
//    auto honst = gethostbyname(host.c_str());
////    for (int i = 0; i < honst->h_length; ++i) {
//        char ip[16];
//        inet_ntop(AF_INET,honst->h_addr_list[0],ip, sizeof(ip));
//        std::cout << ip << std::endl;
////    }
//    int sock = socket(AF_INET, SOCK_STREAM, 0);
//    sockaddr_in addr{};
//    addr.sin_family = AF_INET;
//    addr.sin_port = htons(80);
//    inet_pton(AF_INET, ip,&addr.sin_addr);
//    if (connect(sock, (sockaddr*)(&addr), sizeof(addr))) {
//            char buff[64];
//        int offset = 0;
//        int len;
//        while((len = read(sock, buff, sizeof(buff)))>0) {
//            if(len == 0) {
//                break;
//            }
//        offset += len;
//        buff[offset] = 0;
//        }
//        std::cout << buff << std::endl;
//    } else {
//        std::cout << "error" << std::endl;
//    }

    httplib::Server server;
//    server.listen()
    return 0;
}