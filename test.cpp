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

int main() {
  vector<shared_ptr<B>> vcc(3);
  for (int kI = 0; kI < 3; ++kI){
    vcc[kI] = make_shared<B>();
  }
  decltype(vcc) vbb;
  vbb.swap(vcc);
  cout << vcc.size() <<endl;
}