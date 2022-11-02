#include <iostream>

#include "Widget.h"

void* operator new(size_t size) {
  std::cout << "Allocated " << size << " bytes\n";
  void* ptr = malloc(size);
  return ptr;
}

void operator delete(void* ptr) { std::cout << "Deleted " << ptr << std::endl; }

int main() {
  std::cout << "=== Rule of Five ===\n";

  // Make New
  Widget::Widget w0 = Widget::Widget(21);

  // Copy Constructor
  Widget::Widget w1 = w0;

  // Move constructor
  Widget::Widget w2 = std::move(w1);

  // Copy assign
  w2 = w0;

  // Move assignment
  w2 = std::move(w0);

  // Destructors
  w0.print();
  w1.print();
  w2.print();
  return 0;
}