//
// Created by tuffy on 2020/5/6.
//

#include <iostream>
#include <memory>
#include <epoll/decoder.h>
#include <unordered_map>
#include <vector>
#include <map>
#include <cstring>
using namespace std;

class B {
 public:
  virtual void print() {
    cout << "B" <<endl;
  }
  virtual ~B() {
    cout << "destroy B" << endl;
  }
  std::shared_ptr<int> int_ptr;
  int i = 0;
};

class C : public B {

 public:
  C(int j) {
    i = j;
  }
  void print() {
    cout << "C" << endl;
  }
  ~C() override {
    cout << "destroy C" << endl;
  }
};
std::shared_ptr<B> test() {
  return std::make_shared<C>(10);
}
class E {
 public:
  E(int i) : i(i) {}
  int i;

  virtual void print(){
    cout << "E" << endl;
  }
};

class F : public E {
 public:
  F(int i) : E(i) {}
  void print() override {
    cout << "F" << endl;
  }
};
bool test(int& i){
  i++;
  return false;
}

int main() {
  int i = 0;
  bool flag = test(i) & test(i);
  cout <<i  << endl;
}