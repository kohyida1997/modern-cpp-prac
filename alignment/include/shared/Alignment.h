#pragma once

namespace Alignment {

void sayHelloWorld() noexcept;

class OneCacheLiner {
 public:
  void print() noexcept;
};

struct StructWithPadding {
  int x_;
  double y_;
  int a_;

  void printSize() noexcept;
};

struct StructWithoutPadding {
  double y_;
  int x_;
  int a_;

  void printSize() noexcept;
};

struct Test {
  int x_;
  double y_;
  char array_[3];
  char c_;
};

}  // namespace Alignment