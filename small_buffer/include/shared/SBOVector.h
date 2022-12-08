#pragma once

#include <stdio.h>

#include <array>
#include <iostream>
#include <iterator>

namespace MySBOContainers {

using size_t = std::size_t;

// Guarantees that if the SIZE (number of elements) of this container
// is lesser than OR equal to the StaticCapacity, we will store the
// container elements on the stack instead of the heap.
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
  SBOVector(const SBOVector& other) {
    // Check if the other capacity is on the heap
    if (other.capacity() > StaticCapacity) {
      _dataPtr = new T[other.capacity()];
    } else {
      _dataPtr = &_data[0];
    }

    _capacity = other.capacity();
    _size = other.size();

    std::copy(other._dataPtr, other._dataPtr + other.size(), _dataPtr);
  }

  // Copy Assign
  // Semantics:
  //  - Guarantees elements stored on stack if copied-from is also on stack
  //  - Preserves existing capacity except when copied-from is on stack and
  //    existing capacity is more than StaticCapacity
  //  - Avoids heap re-allocation wherever possible
  //  - If heap re-allocation is required due to insufficient capacity,
  //    the new capacity will be the copied-from capacity.
  //
  SBOVector& operator=(const SBOVector& other) {
    // Case: Other is within stack space, we already allocated on the heap
    // We need to free and store on stack. We don't preserve capacity in this case
    if (other.size() <= StaticCapacity && _capacity > StaticCapacity) {
      delete[](_dataPtr);
      _capacity = StaticCapacity;
    }

    // Case: Other is beyond stack space, our capacity is not enough
    // We need to re-allocate
    else if (other.size() > StaticCapacity && _capacity < other.size()) {
      // If we allocated, we need to free
      if (_capacity > StaticCapacity) {
        delete[](_dataPtr);
      }

      // Just take the capacity of the other vector
      _dataPtr = new T[other.capacity()];
      _capacity = other.capacity();
    }

    // Case: Other is beyond stack space, our capacity is enough
    /* Don't free or reallocate, keep our existing capacity */

    // Case: Other is within stack space, we didn't allocate on the heap
    /* Don't free or reallocate, keep our existing capacity */

    _size = other.size();
    // Copy the data over
    std::copy(other._dataPtr, other._dataPtr + other.size(), _dataPtr);
    return *this;
  }

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
