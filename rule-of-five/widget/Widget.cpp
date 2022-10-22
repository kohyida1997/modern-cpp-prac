#include "Widget.h"

#include <stdio.h>

#include <cstring>
#include <string>

namespace Widget {

Widget &Widget::operator=(Widget &&other) noexcept {
  if (other._resource != nullptr) {
    delete _resource;
    _resource = other._resource;
    other._resource = nullptr;
  }
  printf("[Move-Assignment] Widget [%p] with Resource [%p]\n", this, _resource);
  return *this;
}

Widget::Widget() {
  this->_resource = new int{};
  printf("[Constructor] Widget [%p] with Resource [%p] created\n", this,
         _resource);
}

Widget::Widget(const int &i) {
  this->_resource = new int{i};
  printf("[Constructor] Widget [%p] with Resource [%p] created\n", this,
         _resource);
}

Widget::~Widget() {
  delete _resource;
  printf("[Destructor] Widget [%p] with Resource [%p]\n", this, _resource);
}

Widget::Widget(Widget &&other) noexcept {
  if (other._resource != nullptr) {
    _resource = other._resource;
    other._resource = nullptr;
  }
  printf("[Move-Constructor] Widget [%p] with Resource [%p]\n", this,
         _resource);
}

Widget &Widget::operator=(const Widget &other) {
  if (this != &other) {
    // copy the resource by value
    std::memcpy(_resource, other._resource, sizeof(int));
  }
  printf("[Copy-Assignment] Widget [%p] with Resource [%p]\n", this, _resource);
  return *this;
}

Widget::Widget(const Widget &other) {
  _resource = new int(*other._resource);
  printf("[Copy-Constructor] Widget [%p] with Resource [%p]\n", this,
         _resource);
}

void Widget::print() noexcept {
  printf(">> Widget[addr=%p, resource=%s]\n", this,
         _resource ? std::to_string(*_resource).c_str() : "nullptr");
}

}  // namespace Widget