#include <iostream>
#include <string>
#include <socketutil.h>
#include <unistd.h>


int main()
{
    int sock = SocketUtil::create_socket("0.0.0.0", 8081, [](int sock, addrinfo &result) -> bool {
        bind(sock, result.ai_addr, static_cast<socklen_t>(result.ai_addrlen));
        listen(sock, 100);
        return true;
    });
    int size = 1024;
    char read_buff[size];
    while(true)
    {
        sockaddr_in remoteAddr{};
        socklen_t len;
        int acsock = accept(sock, (sockaddr*)(&remoteAddr), &len);
        if(acsock > 0)
        {

            int len_r;
            while ((len_r = ::read(acsock, read_buff, size)) > 0) {
                std::cout<< std::string(read_buff,len_r) << std::endl;
                write(acsock, read_buff, len_r);
            }
            close(acsock);
        }
    }
    return 0;
}