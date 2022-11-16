#pragma once

#include <stdio.h>

#include <iostream>

#define DEBUG_PRINT 0

namespace MySmartPtrs {

template <typename T>
class SingleThreadSharedPtr {
 private:
  T* _resource;
  unsigned int* _refCount;

 public:
  // Basic constructor
  SingleThreadSharedPtr(T* resource) noexcept : _resource(resource) {
#if DEBUG_PRINT
    std::cout << "Constructor\n";
#endif
    _refCount = new unsigned int(1);
  }

  // Copy constructor (NOT THREAD SAFE)
  SingleThreadSharedPtr(const SingleThreadSharedPtr& other) noexcept
      : _resource(other._resource), _refCount(other._refCount) {
#if DEBUG_PRINT
    std::cout << "Copy Constructor\n";
#endif
    (*_refCount)++;
  }

  // Copy assign (NOT THREAD SAFE)
  SingleThreadSharedPtr& operator=(
      const SingleThreadSharedPtr& other) noexcept {
#if DEBUG_PRINT
    std::cout << "Copy Assign\n";
#endif
    (*_refCount)--;

    // Delete when refCount hits zero for current resource
    if (*_refCount <= 0) {
      delete _resource;
      delete _refCount;
    }

    _resource = other._resource;
    _refCount = other._refCount;
    (*_refCount)++;
    return *this;
  }

  // Move construct
  SingleThreadSharedPtr(SingleThreadSharedPtr&& other)
      : _resource(other._resource), _refCount(other._refCount) {
#if DEBUG_PRINT
    std::cout << "Move construct\n";
#endif
    other._resource = nullptr;
    other._refCount = nullptr;
  }

  // // Move assign
  SingleThreadSharedPtr& operator=(SingleThreadSharedPtr&& other) {
#if DEBUG_PRINT
    std::cout << "Move asssign\n";
#endif
    *_refCount -= 1;
    if (*_refCount <= 0) {
      delete _resource;
      delete _refCount;
    }
    _resource = other._resource;
    _refCount = other._refCount;

    other._refCount = nullptr;
    other._resource = nullptr;

    return *this;
  }

  // Deleter
  ~SingleThreadSharedPtr() {
#if DEBUG_PRINT
    std::cout << this << " called Destructor\n";
#endif
    if (_refCount == nullptr) {
      _resource = nullptr;
      return;
    }

    (*_refCount)--;
    if (*_refCount <= 0) {
      if (_resource != nullptr) delete _resource;
      if (_refCount != nullptr) delete _refCount;
    }
    _resource = nullptr;
  }

  T* get() const { return _resource; }

  auto refCount() { return *_refCount; }

  inline void printRefCountAndResource() noexcept {
    std::printf(
        ">>> [Resource = %p, RefCount = %s]\n", _resource,
        _refCount == nullptr ? "(nil)" : std::to_string(*_refCount).c_str());
  }

  T operator*() { return *_resource; }
};

}  // namespace MySmartPtrs
