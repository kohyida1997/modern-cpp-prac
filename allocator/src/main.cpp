#include <cassert>
#include <iostream>
#include <memory>

#include "shared/BasicAllocator.h"
#include "shared/BasicArena.h"

// Override global new and delete
void* operator new(size_t size) {
  void* ptr = malloc(size);
  std::cout << "Heap Alloc: " << size << " bytes at " << ptr << "\n";
  return ptr;
}

void* operator new(std::size_t count, std::align_val_t al) {
  void* ptr = malloc(count);
  std::cout << "Heap Alloc: " << count << " bytes at " << ptr << " with align ["
            << static_cast<size_t>(al) << "]\n";
  return ptr;
}

void operator delete(void* ptr) { std::cout << "Deleted " << ptr << std::endl; }

void printHeader(const char* s) { std::cout << "\n=== " << s << " ===\n"; }

// Test function prototypes
void testAllocateSingleInt();
void testAllocateFourCharMaxAlign();
void testAllocateManyCharNaturalAlign();
void testAllocateArenaTooSmall();

int main() {
  // Say hi
  BasicAllocator::printBasicAllocatorHeader();
  testAllocateSingleInt();
  testAllocateFourCharMaxAlign();
  testAllocateManyCharNaturalAlign();
  testAllocateArenaTooSmall();
  return 0;
}

void testAllocateSingleInt() {
  printHeader(__func__);
  auto arena = Arena::BasicArena<Arena::max_alignment>();
  int* intPtrX = new (arena.allocate(sizeof(int))) int(25);
  assert(intPtrX != nullptr);
  assert(*intPtrX == 25);
}

void testAllocateFourCharMaxAlign() {
  printHeader(__func__);
  constexpr size_t sz = Arena::max_alignment * 4;
  auto arena = Arena::BasicArena<sz>();
  for (int i = 0; i < 4; i++) {
    char* charPtr = new (arena.allocate(sizeof(char))) char('c');
    assert(charPtr != nullptr);
    assert(*charPtr == 'c');
  }
  // Last char has 15 bytes after it
  assert(arena.available_size() == (sz - 3 * Arena::max_alignment - 1));
  assert(arena.used() == (sz - arena.available_size()));

  std::cout << ">>> Expect to see Heap Alloc:\n";
  char* charPtr = new (arena.allocate(sizeof(char))) char('d');
  assert(charPtr != nullptr);
  assert(*charPtr == 'd');
  assert(!arena.in_buffer(reinterpret_cast<std::byte*>(charPtr)));
  std::cout << ">>> Expect to see global delete called for charPtr["
            << static_cast<void*>(charPtr) << "]\n";
  arena.deallocate(reinterpret_cast<std::byte*>(charPtr), sizeof(char));
}

void testAllocateManyCharNaturalAlign() {
  printHeader(__func__);
  constexpr int count = 1024;
  constexpr size_t sz = Arena::max_alignment * static_cast<size_t>(1024);
  auto arena = Arena::BasicArena<sz>();
  // Allocate with max align
  for (int i = 0; i < count; i++) {
    char* charPtr = new (arena.allocate(sizeof(char))) char('c');
    assert(charPtr != nullptr);
    assert(*charPtr == 'c');
    assert(arena.in_buffer(reinterpret_cast<std::byte*>(charPtr)));
  }

  // Last char has 15 bytes after it
  assert(arena.available_size() ==
         (sz - (count - 1) * Arena::max_alignment - 1));
  assert(arena.used() == (sz - arena.available_size()));

  // Allocate remaining using natural align
  for (int i = arena.available_size(); i > 0; i--) {
    auto* ptr = new (arena.allocate(sizeof(char), alignof(char))) char(i);
    assert(ptr != nullptr);
    assert(*ptr == i);
    assert(arena.in_buffer(reinterpret_cast<std::byte*>(ptr)));
  }

  // Assert everything used up
  assert(arena.used() == arena.capacity() && arena.available_size() == 0);
}

void testAllocateArenaTooSmall() {
  printHeader(__func__);
  // Too Small even for int
  constexpr size_t sz = sizeof(int) / 2;
  auto arena = Arena::BasicArena<sz>();
  // Try to allocate anyway
  int* intPtr = new (arena.allocate(sizeof(int), alignof(int))) int(12345);
  std::cout << ">>> Expect to see global delete called for intPtr["
            << static_cast<void*>(intPtr) << "]\n";

  // Should not exist in the arena
  assert(!arena.in_buffer(reinterpret_cast<std::byte*>(intPtr)));
  assert(arena.available_size() == sizeof(int) / 2);
  assert(arena.used() == 0);
  arena.deallocate(reinterpret_cast<std::byte*>(intPtr), sizeof(int));
}
