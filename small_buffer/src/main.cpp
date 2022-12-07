#include <stdio.h>  // std::printf

#include <cassert>
#include <iostream>
#include <utility>  // std::pair

#include "Utils.h"             // Func header macros
#include "shared/SBOVector.h"  // SBOVector

/* Debug options */
#define OVERLOAD_NEW_DELETE 0  // Will cause Valgrind to break if set to 1

/* Overload global operator new and delete */
#if OVERLOAD_NEW_DELETE
static unsigned int HEAP_BYTES_ALLOCATED = 0;
void* operator new(size_t s) {
  auto ptr = malloc(s);
  HEAP_BYTES_ALLOCATED += s;
  std::cout << "New: Heap Allocated " << s << " bytes at " << ptr << std::endl;
  return ptr;
}

void operator delete(void* ptr) {
  free(ptr);
  std::cout << "Delete: Free " << ptr << std::endl;
}
#endif

/* Types and Namespaces */
using namespace Utils;

using size_t = std::size_t;

template <typename T>
using SBOVector = MySBOContainers::SBOVector<T>;

template <typename T, size_t F>
using SBOVectorWithInitSize = MySBOContainers::SBOVector<T, F>;

/* Function prototypes */
void testStackAllocationOnly();
void testStackAllocationOverrideDefaultStaticCapacity();
void testExceedStackCapacity();

// Main Driver code
int main() {
  // Say Hello Y'all
  PRINT_MODULE_HEADER();
  testStackAllocationOnly();
  testStackAllocationOverrideDefaultStaticCapacity();
  testExceedStackCapacity();
  return 0;
}
/* Function definitions*/
void testStackAllocationOnly() {
  PRINT_FUNC_HEADER(__func__);
  using data_t = double;
  using st = std::size_t;

  // Default Stack Size is 16 elements
  auto myVec = SBOVector<data_t>();

  // Push_back 6 elements
  st count = 6;
  for (st i = 0; i < count; i++) myVec.push_back(1.01 * i);
  printVectorRange(myVec, 0, count);

  // Push_back 10 more elements
  st more = 10;
  for (st i = 0; i < more; i++) myVec.push_back(1.01 * (i + count));
  printVectorRange(myVec, 0, more + count);

#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == 0);
#endif
  PRINT_TEST_PASS(__func__);
}

void testStackAllocationOverrideDefaultStaticCapacity() {
  PRINT_FUNC_HEADER(__func__);
  using data_t = std::pair<short, double>;
  using st = std::size_t;

  constexpr st count = 8;
  auto v = SBOVectorWithInitSize<data_t, count>();
  for (size_t i = 0; i < count; i++)
    v.push_back(std::make_pair(i + 1, 1.0 * (i + 1) + 0.1));
  printVectorRange(v, 0, count, [](const data_t& p) {
    std::cout << "[" << p.first << ", " << p.second << "]";
  });

#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == 0);
#endif
  PRINT_TEST_PASS(__func__);
}

void testExceedStackCapacity() {
  PRINT_FUNC_HEADER(__func__);
  using st = std::size_t;

  // Default Stack Size is 4 elements
  auto myVec = SBOVectorWithInitSize<int, 4>();

  // Push_back 4 elements
  st count = 4;
  for (st i = 0; i < count; i++) myVec.push_back(i + 1);
  printVector(myVec);  // 4 elements

  // Expect re-allocation, fallback to heap
  myVec.push_back(count + 1);  // 5 elements

#if OVERLOAD_NEW_DELETE
  int usedSoFar = sizeof(int) * count << 1;
  assert(HEAP_BYTES_ALLOCATED == usedSoFar);
#endif

  for (st i = 1; i < count; i++) myVec.push_back(count + i + 1);
  printVector(myVec);  // 8 elements

  // Expect re-allocation AGAIN
  myVec.push_back(count * 2 + 1);
  for (st i = count * 2 + 2; i < count * 2 * 2; i++) myVec.push_back(i);
  printVector(myVec);

#if OVERLOAD_NEW_DELETE
  usedSoFar += sizeof(int) * count << 2;
  assert(HEAP_BYTES_ALLOCATED == usedSoFar);
#endif
  PRINT_TEST_PASS(__func__);
}