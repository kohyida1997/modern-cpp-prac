#pragma once

#include <cstddef>
#include <iostream>
#include <memory>

namespace BasicArena {

// CAP is the capacity in bytes of the arena
template <size_t T>
// Basic Arena that allocates a memory pool on the Heap
class BasicArena {
  static constexpr size_t max_alignment = alignof(std::max_align_t);

 public:
  BasicArena() {
    _buffer =
        new (std::align_val_t(max_alignment)) std::byte[T];  // On The Heap
    _ptr = _buffer;

    void* check = reinterpret_cast<void*>(_buffer);
    void* initial = check;
    size_t sz = T;
    void* newPtr = std::align(max_alignment, T, check, sz);
    std::cout << "Is Aligned? "
              << (newPtr == initial ? "true" : "false")
              << "\n";
    // std::cout << "Adjusted " << newPtr << "\n";
  };

  ~BasicArena() {
    delete _buffer;
    _ptr = nullptr;
    _buffer = nullptr;
  }
  BasicArena(const BasicArena&) = delete;             // Delete copy constructor
  BasicArena& operator=(const BasicArena&) = delete;  // Delete Copy Assignment

  void* allocate(size_t s) { return nullptr; }

 private:
  std::byte* _buffer;
  std::byte* _ptr;
};

}  // namespace BasicArena