//
// Created by tuffy on 2020/5/6.
//

#include <iostream>
#include <memory>
#include <boost/optional.hpp>
using namespace std;

class B {
 public:
 std::shared_ptr<int> int_ptr;
 int i = 0;
};
int &tt(const B &b) {
  return const_cast<int&>(b.i);
}

int main() {
  boost::optional<B> op;
  cout << op.has_value() <<endl;
  op.emplace(B());
  op.;
  op->i = 1;
  cout << op->i <<endl;
}