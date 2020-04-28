//
// Created by tuffy on 2020/4/23.
//
#include <sys/select.h>

#include <iostream>
#include <functional>
#include <map>
#include <memory>
#include <thread>
#include <csignal>
#include <vector>

#include "base_server.h"
//#include <boost/atomic.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#define PRINT(a) (std::cout << a << std::endl)

using namespace std;
class Test {
 public:
  Test(int a) : a(a) {}
  int a = 0;
};
class A {
 public:

  A(int i) : t(i) {};

  virtual void print();
  void work(const function<void()> &fun){
    fun();
  }
  const Test &GetT() const;
  Test t;
};
boost::lockfree::spsc_queue<int> kQueue(3);
void A::print() {
  PRINT(t.a);
  t.a = 100;
}
const Test &A::GetT() const {
  return t;
}
class B : public A {

 public:
  void print() override {
    PRINT("b");
  }
};

void print(int signal){
  PRINT(std::this_thread::get_id());
}



#define WAKE 255
void test(Test &t){
  t.a = 10000;
}
int main(){
  A a(1);
}