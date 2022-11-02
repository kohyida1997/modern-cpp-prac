#pragma once

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>

namespace Arena {
constexpr size_t max_alignment = alignof(std::max_align_t);
// T is the capacity in bytes of the arena
template <size_t T>
// Basic Arena that allocates a stack-like memory pool on the Heap,
// monotonically. Pretty dumb yeah
class BasicArena {
 public:
  BasicArena() noexcept {
    _buffer =
        new (std::align_val_t(max_alignment)) std::byte[T];  // On The Heap
    _ptr = _buffer;
  };

  ~BasicArena() {
    delete _buffer;
    _ptr = nullptr;
    _buffer = nullptr;
  }

  // Cannot be copy-constructed, it represents a physical location on the RAM
  BasicArena(const BasicArena&) = delete;  // Delete copy constructor

  // Arena cannot be copy-assigned, it represents a physical location on the RAM
  BasicArena& operator=(const BasicArena&) = delete;  // Delete Copy Assignment

  // Arena cannot be move-assigned, it represents a physical location on the RAM
  BasicArena& operator=(BasicArena&&) = delete;  // Delete Move Assignment

  // Arena cannot be move-constructed, it represents a physical location on the
  // RAM
  BasicArena(BasicArena&&) = delete;  // Delete Move constructor

  void* allocate(size_t s, size_t align) {
    if (available_size() == 0ul) {
      std::cout << "Buffer available size is zero\n";
      return reinterpret_cast<void*>(new std::byte[s]);
    }

    // Copy the pointer
    auto* obj = reinterpret_cast<void*>(_ptr);
    auto sz = available_size();
    // Align it
    obj = std::align(align, s, obj, sz);

    // Fail to align/out of space, just malloc it
    if (obj == nullptr) {
      std::cout << "Align failed - nullptr, allocating on Heap instead\n";
      return reinterpret_cast<void*>(new std::byte[s]);
    }

    // Cast to byte
    auto* obj_byte_ptr = reinterpret_cast<std::byte*>(obj);

    // This should be technically not necessary
    if (!in_buffer(obj_byte_ptr + s - 1)) {
      std::cout << "Ran out of space in buffer\n";
      return reinterpret_cast<void*>(new std::byte[s]);
    }

    // Okay, advance the pointer
    _ptr = obj_byte_ptr + s;
    return obj_byte_ptr;
  }

  inline void* allocate(size_t s) noexcept {
    return allocate(std::move(s), max_alignment);
  }

  // TODO: If ptr is at end of stack, move ptr back to reclaim space
  void deallocate(std::byte* ptr, size_t x) noexcept {
    if (!in_buffer(ptr)) delete ptr;
  }

  bool in_buffer(std::byte* test) noexcept {
    return std::uintptr_t(test) < std::uintptr_t((_buffer + T)) &&
           std::uintptr_t(test) >= std::uintptr_t(_buffer);
  }

  size_t capacity() { return T; }

  size_t available_size() { return std::max(T - (_ptr - _buffer), 0uL); }

  size_t used() { return _ptr - _buffer; }

 private:
  // Pointer to start of the buffer
  std::byte* _buffer;
  // Pointer to next available location in buffer
  std::byte* _ptr;
};

}  // namespace Arena
