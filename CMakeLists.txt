cmake_minimum_required(VERSION 3.15)
project(agent)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")

set(base_server base_server.h base_server.cpp)
add_executable(bio_server server.cpp ${base_server})

set(common select/buffer.h select/buffer.cpp select/context.cpp select/context.h epoll/decoder.h epoll/decoder.cpp)
set(select_single_server select/select_server.h select/select_server.cpp ${base_server} ${common})

add_executable(select_single_run select.cpp ${select_single_server})

set(select_multi_server select/thread/multi_thread_select_server.cpp select/thread/multi_thread_select_server.h
        ${select_single_server})

add_executable(select_multi_server_run select/thread/main.cpp ${select_multi_server})
add_executable(test test.cpp ${common})

set(epoll_server epoll/epoll_server.h epoll/epoll_server.cpp ${common} ${base_server} epoll/channal_pipe_context.cpp epoll/channal_pipe_context.h)
add_executable(epoll_server_run epoll/main.cpp ${epoll_server})

add_executable(hello_server test/hello_server.cpp)
add_executable(sendfile_test test/sendfile_test.cpp)