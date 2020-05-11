cc_library(
    name = "epoll_server",
#    hdrs = ["decoder.h", "epoll_server.h", "channal_pipe_context.h"],
#    srcs = ["decoder.cpp", "epoll_server.cpp", "channal_pipe_context.cpp"],
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
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