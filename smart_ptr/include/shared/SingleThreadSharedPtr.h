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

  // Increase ref count if we are owning a resource
  void tryIncreaseRefCount() noexcept {
    if (_refCount) {
      (*_refCount)++;
    }
  }

  // Decrease ref count if we are owning a resource
  void tryDecreaseRefCount() noexcept {
    if (_refCount) {
      (*_refCount)--;
      if (*_refCount == 0) {
        delete _refCount;
        delete _resource;
      }
    }
  }

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
    tryIncreaseRefCount();
  }

  // Copy assign (NOT THREAD SAFE)
  SingleThreadSharedPtr& operator=(
      const SingleThreadSharedPtr& other) noexcept {
#if DEBUG_PRINT
    std::cout << "Copy Assign\n";
#endif
    // Prevent self-assign
    if (this == &other) return *this;

    // Clean up my own resources, we don't own it anymore
    tryDecreaseRefCount();

    // Set our resource to the other's
    _resource = other._resource;
    _refCount = other._refCount;

    // If needed, we increase the ownership of ref count
    tryIncreaseRefCount();
    return *this;
  }

  // Move construct (NOT THREAD SAFE)
  SingleThreadSharedPtr(SingleThreadSharedPtr&& other)
      : _resource(other._resource), _refCount(other._refCount) {
#if DEBUG_PRINT
    std::cout << "Move construct\n";
#endif

    // Invalidate the other's resources
    other._resource = nullptr;
    other._refCount = nullptr;
  }

  // // Move assign (NOT THREAD SAFE)
  SingleThreadSharedPtr& operator=(SingleThreadSharedPtr&& other) {
#if DEBUG_PRINT
    std::cout << "Move assign\n";
#endif
    // Prevent self-assign
    if (this == &other) return *this;

    // Clean up my own resources, we don't own it anymore
    tryDecreaseRefCount();

    // Steal the other's resources
    _resource = other._resource;
    _refCount = other._refCount;

    // Invalidate other's resources
    other._refCount = nullptr;
    other._resource = nullptr;

    return *this;
  }

  // Destructor (NOT THREAD SAFE)
  ~SingleThreadSharedPtr() {
#if DEBUG_PRINT
    std::cout << this << " called Destructor\n";
#endif
    // Clean up my own resources
    tryDecreaseRefCount();
  }

  T* get() const noexcept { return _resource; }

  /*
    refCount() returns the number of shared_ptrs that have a reference
    to the resource. If the resource is null or invalid (such as after a move
    operation), it will throw
  */
  auto refCount() { return *_refCount; }

  bool hasResource() noexcept {
    return _refCount != nullptr && _resource != nullptr;
  }

  inline void printRefCountAndResource() noexcept {
    std::printf(
        ">>> [Resource = %p, RefCount = %s]\n", _resource,
        _refCount == nullptr ? "(nil)" : std::to_string(*_refCount).c_str());
  }

  T operator*() { return *_resource; }
};

}  // namespace MySmartPtrs
