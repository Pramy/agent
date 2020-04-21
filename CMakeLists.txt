cmake_minimum_required(VERSION 3.15)
project(agent)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(/Users/tuffy/Desktop/github/cpp-httplib)
include_directories(/Users/tuffy/Desktop/github/sion/Sion)
include_directories(/Users/tuffy/Desktop/github/mongoose)

add_executable(server server.cpp socketutil.cpp socketutil.h)
add_executable(client main.cpp socketutil.cpp socketutil.h)
add_executable(moo moo.cpp socketutil.cpp socketutil.h)
add_executable(read read.cpp socketutil.cpp socketutil.h)