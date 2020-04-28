# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
    name = "test",
    srcs = ["test.cpp"],
    deps = [":base_server", "@boost//:lockfree", "@boost//:iterator"],

)
cc_binary(
    name = "select_multi_server_run",
    srcs = ["select/thread/main.cpp"],
    deps = [":select_multi_server"],
)
cc_library(
    name = "select_multi_server",
    srcs = ["select/thread/multi_thread_select_server.cpp"],
    hdrs = ["select/thread/multi_thread_select_server.h"],
    deps = [
        ":select_single_server",
        "@boost//:lockfree",
        "@boost//:iterator",
    ],

)
cc_library(
    name = "base_server",
    srcs = ["base_server.cpp"],
    hdrs = ["base_server.h"],
)

cc_binary(
    name = "bio_server",
    srcs = ["server.cpp"],
    copts = [
        "--std=c++11",
    ],
    deps = [
        ":base_server",
    ],
)

cc_library(
    name = "common",
    srcs = [
        "select/buffer.cpp",
        "select/context.cpp",
    ],
    hdrs = [
        "select/buffer.h",
        "select/context.h",
    ],
)

cc_library(
    name = "select_single_server",
    srcs = ["select/select_server.cpp"],
    hdrs = ["select/select_server.h"],
    copts = [],
    deps = [
        ":base_server",
        ":common",
    ],
)

cc_binary(
    name = "select_single_run",
    srcs = ["select.cpp"],
    copts = [
        "--std=c++11",
    ],
    deps = [
        ":select_single_server",
    ],
)
# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
# cc_binary(
#   name = "rule unique name",
#   srcs = ["source files"],
#   copts = [],
#   deps = ["libraries to be linked"],
# )

# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
