//
// Created by tuffy on 2020/4/24.
//

#ifndef AGENT_BUFFER_H
#define AGENT_BUFFER_H
#include <unistd.h>
#include <cstring>
class Buffer {

public:
  explicit Buffer(int maxLen);
  virtual ~Buffer();

  bool IsReadable();
  bool IsWriteable();
  bool IsEmpty();
  bool IsFull();
  void Reset();

  int Read(int socket_fd);
  int Read(const char *src, int len);
  int Write(int socket_fd);
  int Write(char *des, int len);

  char *GetReadIndex() const;
  bool SetReadIndex(char *index);
  char *GetWriteIndex() const;
  bool SetWriteIndex(char *index);

  int GetSize();

private:
  char *data;
  int max_len;
  char *read_index;
  char *write_index;
};

#endif // AGENT_BUFFER_H
