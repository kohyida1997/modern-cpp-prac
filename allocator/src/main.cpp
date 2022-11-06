#include <cassert>
#include <iostream>
#include <memory>

#include "shared/BasicAllocator.h"
#include "shared/BasicArena.h"

// Override global new and delete
void* operator new(size_t size) {
  void* ptr = std::malloc(size);
  std::cout << "Heap Alloc: " << size << " bytes at " << ptr << "\n";
  return ptr;
}

void* operator new(std::size_t count, std::align_val_t al) {
  void* ptr = std::malloc(count);
  std::cout << "Heap Alloc: " << count << " bytes at " << ptr << " with align ["
            << static_cast<size_t>(al) << "]\n";
  return ptr;
}

void operator delete(void* ptr) {
  std::free(ptr);
  std::cout << "Deleted " << ptr << std::endl;
}

void operator delete[](void* ptr) {
  std::free(ptr);
  std::cout << "Deleted[] " << ptr << std::endl;
}

// Helpers
template <typename T>
constexpr std::byte* unsafeCastToBytePtr(T* ptr) {
  return reinterpret_cast<std::byte*>(ptr);
}

void printHeader(const char* s) { std::cout << "\n=== " << s << " ===\n"; }

// Test function prototypes
// Tests for allocation only
void testAllocateSingleInt();
void testAllocateFourCharMaxAlign();
void testAllocateManyCharNaturalAlign();
void testAllocateArenaTooSmall();

// Tests for deallocation as well
void testAllocateAndDeallocateSingleChar();
void testAllocateAndDeallocateManyMixed();

int main() {
  // Say hi
  BasicAllocator::printBasicAllocatorHeader();
  testAllocateSingleInt();
  testAllocateFourCharMaxAlign();
  testAllocateManyCharNaturalAlign();
  testAllocateArenaTooSmall();
  testAllocateAndDeallocateSingleChar();
  testAllocateAndDeallocateManyMixed();
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

void testAllocateAndDeallocateSingleChar() {
  printHeader(__func__);
  auto a = Arena::BasicArena<32>();

  // Allocate a single char
  // Expect no heap allocations
  auto&& testChar = 'k';
  char* c = new (a.allocate(sizeof(char), alignof(char))) char(testChar);
  assert(a.in_buffer(unsafeCastToBytePtr(c)));
  assert(a.used() == 1ul);
  assert(*c == testChar);

  // Now deallocate that char from before
  a.deallocate(unsafeCastToBytePtr(c), sizeof(char));
  assert(a.used() == 0ul);
  assert(a.available_size() == a.capacity());
  // Don't expect any global delete called

  // Now reallocate the char, expect it to be in the original position
  testChar = 'q';
  char* c_later = new (a.allocate(sizeof(char), alignof(char))) char(testChar);
  assert(c_later == c);
  assert(a.in_buffer(unsafeCastToBytePtr(c_later)));
  assert(a.used() == 1ul);
  assert(*c_later == 'q');
}

void testAllocateAndDeallocateManyMixed() {
  printHeader(__func__);
  auto a = Arena::BasicArena<1024>();

  // Allocate a single char
  auto&& testChar = 'q';
  auto* c = new (a.allocate(sizeof(char), alignof(char))) char(testChar);
  assert(a.in_buffer(unsafeCastToBytePtr(c)));

  // Allocate an int
  auto&& testInt = 3;
  auto* z = new (a.allocate(sizeof(int), alignof(int))) int(testInt);
  assert(a.in_buffer(unsafeCastToBytePtr(z)));
  assert(*z == testInt);

  // Due to aligned memory allocation, two int worth of space is used
  assert(a.used() == (sizeof(int) * 2));

  // Deallocate the int 'z'
  a.deallocate(unsafeCastToBytePtr(z), sizeof(int));
  // Due to aligned memory allocation, padding for int alignment is wasted
  // and not reclaimed
  assert(a.used() == (sizeof(int) * 1));

  // Now allocate two ints in a row, delete the 2nd last one, expect no-op
  auto* q1 = new (a.allocate(sizeof(int), alignof(int))) int(testInt);
  assert(q1 == z);
  auto* q2 = new (a.allocate(sizeof(int), alignof(int))) int(testInt + 1);
  assert(*q1 == testInt && *q2 == (testInt + 1));
  assert((unsafeCastToBytePtr(q2) - unsafeCastToBytePtr(q1)) == sizeof(int));
  const auto& currUsed = a.used();
  a.deallocate(unsafeCastToBytePtr(q1), sizeof(int));
  assert(currUsed == a.used());

  // Now deallocate the first char, again, expect no-op
  a.deallocate(unsafeCastToBytePtr(c), sizeof(*c));
  assert(currUsed == a.used());
}
