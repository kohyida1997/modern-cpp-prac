#include <stdio.h>

#include <cassert>
#include <iostream>
#include <type_traits>

#include "tests.cpp"

// Debug options
#define OVERLOAD_NEW_DELETE 0

#define PRINT_MODULE_HEADER()                                              \
  std::cout << "=== Welcome to the Smart Pointer submodule. Get ready to " \
               "feel smart! ===\n";

#if OVERLOAD_NEW_DELETE
// Overload global operator new and delete
void* operator new(size_t s) {
  auto ptr = malloc(s);
  std::cout << "New: Heap Allocated " << s << " bytes at " << ptr << std::endl;
  return ptr;
}

void operator delete(void* ptr) {
  free(ptr);
  std::cout << "Delete: Free " << ptr << std::endl;
}
#endif

// Main Driver code
int main() {
  // Say Hello Y'all
  PRINT_MODULE_HEADER();
  Tests::runTest1();
  Tests::runTest2();
  Tests::runTest3();
  return 0;
}
