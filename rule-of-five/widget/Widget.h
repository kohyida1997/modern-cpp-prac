#pragma once

#include <iostream>

namespace Widget {

class Widget {
 public:
  Widget();
  Widget(const int &);
  ~Widget();
  Widget(const Widget &other);
  Widget &operator=(const Widget &other);
  Widget(Widget &&other) noexcept;
  Widget &operator=(Widget &&other) noexcept;

  void print() noexcept;

 private:
  int *_resource;
};

}  // namespace Widget