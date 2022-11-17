#pragma once

#include <stdio.h>

#include <array>
#include <iostream>
#include <iterator>

namespace MySBOContainers {

using size_t = std::size_t;

// TODO: Fallback to heap allocation when size exceeded
template <typename T, size_t StaticCapacity = 16>
class SBOVector {
 private:
  size_t _size;
  size_t _capacity = StaticCapacity;
  std::array<T, StaticCapacity> _data;
  T* _dataPtr;

 public:
  // Constructor
  SBOVector() : _size(0), _dataPtr(&_data[0]) {}

  // Copy Constructor

  // Copy Assign

  // Move Constructor

  // Move Assign

  void push_back(const T& val) {
    if (_size + 1 > _capacity) {
      std::cout << "!!! Push_back failed, exceeded capacity\n";
      throw std::runtime_error("Runtime error detected: SBOVector is full");
    }
    _data[_size++] = val;  // Copy construct T
  }

  T operator[](size_t i) const {
    if (i >= _size) {
      std::printf("Accessing Index %ld but size is %ld\n", i, _size);
      throw std::runtime_error("Index out of bounds!");
    }
    return _dataPtr[i];
  }

  size_t capacity() const { return _capacity; }

  size_t size() const { return _size; }

  // Destructor
  ~SBOVector() {
    if (_capacity > StaticCapacity) {
      free(_dataPtr);
    }
  }

 private:
};

}  // namespace MySBOContainers
