cc_library(
    name = "epoll_server",
    hdrs = ["decoder.h", "epoll_server.h"],
    srcs = ["decoder.cpp", "epoll_server.cpp"],
    deps = [
        "//:common",
        "//:base_server",
        "@boost//:lockfree",
        "@boost//:iterator",
    ],
)

cc_binary(
    name = "epoll_server_run",
    srcs = ["main.cpp"],
    copts= ["-std=c++11"],
    linkopts = ["-pthread"],
    deps = [":epoll_server"],
)