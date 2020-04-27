//
// Created by tuffy on 2020/4/24.
//

#include "buffer.h"

#include <cerrno>
#include <unistd.h>

Buffer::Buffer(int maxLen) : max_len(maxLen) {
  data = new char[maxLen];
  Reset();
}
Buffer::~Buffer() {
  delete [] data;
  data = nullptr;
  read_index = nullptr;
  write_index = nullptr;
}

//inline
bool Buffer::IsReadable() {
  return read_index - data <  max_len;
}

//inline
bool Buffer::IsWriteable() {
  return write_index < read_index;
}

inline
bool Buffer::IsEmpty() {
  return write_index == read_index;
}

inline
void Buffer::Reset() {
  read_index = write_index = data;
}

char *Buffer::GetReadIndex() const { return read_index; }
bool Buffer::SetReadIndex(char *index) {
  if (index< data || index < write_index || index >= data + max_len) {
    return false;
  }
  read_index = index;
  return true;
}
char *Buffer::GetWriteIndex() const { return write_index; }
bool Buffer::SetWriteIndex(char *index) {
  if( index < data || index < read_index || index >= data + max_len) {
    return false;
  }
  write_index = index;
  return true;
}
int Buffer::Read(int socket_fd) {
  int len = ::read(socket_fd, read_index, data + max_len - read_index);
  if (len > 0 ) {
    read_index += len;
  }
  return len;
}
int Buffer::Write(int socket_fd) {
  int len = :: write(socket_fd, write_index, read_index - write_index);
  if (len > 0) {
    write_index += len;
  }
  if (IsEmpty() && write_index != data){
    Reset();
  }
  return len;
}
int Buffer::GetSize() { return read_index - write_index; }
bool Buffer::IsFull() { return read_index == write_index && write_index != data; }
