# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
  name = "hello_server",
  srcs = ["hello_server.cpp"],
  copts= ["-std=c++11"],
  linkopts = ["-lpthread"],
)