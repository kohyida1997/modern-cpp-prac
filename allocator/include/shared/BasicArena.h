#pragma once

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>

namespace Arena {
constexpr size_t max_alignment = alignof(std::max_align_t);

// T is the capacity in bytes of the arena
template <size_t T>
// Basic Arena is a pool that allocates a stack-like memory region on the Heap
// It allocates memory always to the end of the region, and only reclaims memory
// if the de-allocated memory is at the end of the region (like a stack).
//
// It is alignment aware and will pad data accordingly. Due to the design, extra
// bytes used for padding may not be reclaimed.

// Yeah, it is a pretty dumb implementation (just for learning purposes).
class BasicArena {
 public:
  BasicArena() noexcept {
    _buffer =
        new (std::align_val_t(max_alignment)) std::byte[T];  // On The Heap
    _ptr = _buffer;
  };

  ~BasicArena() {
    delete[] _buffer;
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

  // Allocates s bytes according to the given alignment
  // Fall-backs to heap allocation if there is insufficient space
  // or errors
  void* allocate(size_t s, size_t align) {
    if (available_size() == 0ul) {
      std::cout << "Buffer available size is zero\n";
      return static_cast<void*>(::operator new(s));
    }

    // Copy the pointer
    // Use reinterpret_cast as static_cast from byte* to void*
    // is invalid
    auto* obj = reinterpret_cast<void*>(_ptr);
    auto sz = available_size();
    // Align the obj pointer
    obj = std::align(align, s, obj, sz);

    // Fail to align/out of space, default to malloc
    if (obj == nullptr) {
      std::cout << "Align failed - nullptr, allocating on Heap instead\n";
      return static_cast<void*>(::operator new(s));
    }

    // Cast to byte
    auto* obj_byte_ptr = reinterpret_cast<std::byte*>(obj);

    // This is technically not necessary because std::align
    // accounts for insufficient space
    if (!in_buffer(obj_byte_ptr + s - 1)) {
      std::cout << "Ran out of space in buffer\n";
      return static_cast<void*>(::operator new(s));
    }

    // Okay, advance the pointer
    _ptr = obj_byte_ptr + s;
    return obj_byte_ptr;
  }

  // No align specified, default to max alignment
  // Just inline-it
  inline void* allocate(size_t s) noexcept {
    return allocate(std::move(s), max_alignment);
  }

  // If ptr is at end of region (stack-like), move back ptr to reclaim
  // WARNING: Does not reclaim space due to alignment
  void deallocate(std::byte* ptr, size_t x) noexcept {
    // If it is not in the buffer, simply call global delete
    if (!in_buffer(ptr)) {
      delete ptr;
      return;
    }

    // Check if it is the last one
    if ((ptr + x) == _ptr) {
      // Move the curr _ptr back
      _ptr = ptr;
    }
  }

  bool in_buffer(std::byte* test) noexcept {
    // The book uses std::uintptr_t, but I think std::byte is OK
    // according to below:
    // https://stackoverflow.com/questions/52761440/should-stdbyte-pointers-be-used-for-pointer-arithmetic
    return test < (_buffer + T) && test >= _buffer;
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
