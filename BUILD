
# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary


cc_library(
  name = "socket_util",
  srcs = ["socketutil.cpp"],
  hdrs = ["socketutil.h"],
)

cc_binary(
    name = "server",
    srcs = ["server.cpp"],
    deps = [
      ":socket_util"
    ],
    copts = [
   "--std=c++11",
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
