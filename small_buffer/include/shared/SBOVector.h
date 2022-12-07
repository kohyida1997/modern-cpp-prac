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
  T* _dataPtr = nullptr;

 public:
  // Constructor
  SBOVector() : _size(0), _dataPtr(&_data[0]) {}

  // Copy Constructor

  // Copy Assign

  // Move Constructor

  // Move Assign

  void push_back(const T& val) {
    if (_size + 1 > _capacity) {
      if (_size == StaticCapacity)
        std::cout
            << "!!! Stack Capacity Exceeded, falling back to heap allocation\n";

      std::size_t newCapacity = _capacity == 0 ? 1 : _capacity * 2;
      // Allocate a new chunk
      T* temp = reinterpret_cast<T*>(new T[newCapacity]);

      // Copy over
      if (_capacity > StaticCapacity) {  // Already using heap data
        std::move(_dataPtr, _dataPtr + _size, temp);
        delete[] (_dataPtr);
      } else {
        std::move(_data.begin(), _data.end(), temp);
      }
      _capacity = newCapacity;
      _dataPtr = temp;
    }
    _dataPtr[_size++] = val;  // Copy construct T
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
      delete[] (_dataPtr);
    }
  }

 private:
};

}  // namespace MySBOContainers
