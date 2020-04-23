
# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary


cc_library(
  name = "base_server",
  srcs = ["base_server.cpp"],
  hdrs = ["base_server.h"],
)

cc_binary(
    name = "bio_server",
    srcs = ["server.cpp"],
    deps = [
      ":base_server"
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
