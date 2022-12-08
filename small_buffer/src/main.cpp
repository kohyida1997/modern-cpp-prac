#include <stdio.h>  // std::printf

#include <cassert>
#include <iostream>
#include <utility>  // std::pair
#include <vector>

#include "Utils.h"             // Func header macros
#include "shared/SBOVector.h"  // SBOVector

/* Debug options note: */
// Overloading New and Delte will cause Valgrind to break

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
void testDefaultCapacityZero();
void testCopyConstructor();
void testCopyAssign();

// Main Driver code
int main() {
  // Say Hello Y'all
  PRINT_MODULE_HEADER();
  testStackAllocationOnly();
  testDefaultCapacityZero();
  testStackAllocationOverrideDefaultStaticCapacity();
  testExceedStackCapacity();
  testCopyConstructor();
  testCopyAssign();

  return 0;
}

/* Function definitions*/
void testStackAllocationOnly() {
  PRINT_FUNC_HEADER(__func__);
#if OVERLOAD_NEW_DELETE
  HEAP_BYTES_ALLOCATED = 0;
#endif
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
#if OVERLOAD_NEW_DELETE
  HEAP_BYTES_ALLOCATED = 0;
#endif
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
#if OVERLOAD_NEW_DELETE
  HEAP_BYTES_ALLOCATED = 0;
#endif
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

void testDefaultCapacityZero() {
  PRINT_FUNC_HEADER(__func__);
#if OVERLOAD_NEW_DELETE
  HEAP_BYTES_ALLOCATED = 0;
#endif
  using data_t = uint64_t;

  // Default Stack Size is 0 elements
  auto myVec = SBOVectorWithInitSize<data_t, 0>();
  assert(myVec.size() == 0);
  assert(myVec.capacity() == 0);

  // First allocation -> capacity becomes 1
  myVec.push_back(1);
  printVector(myVec);
  assert(myVec.size() == 1);
  assert(myVec.capacity() == 1);
#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == sizeof(data_t) * 1);
#endif

  // Second allocation -> capacity becomes 2
  myVec.push_back(2);
  printVector(myVec);
  assert(myVec.size() == 2);
  assert(myVec.capacity() == 2);
#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == sizeof(data_t) * (1 + 2));
#endif

  // Third allocation -> capacity becomes 4
  myVec.push_back(3);
  myVec.push_back(4);
  printVector(myVec);
  assert(myVec.size() == 4);
  assert(myVec.capacity() == 4);
#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == sizeof(data_t) * (1 + 2 + 4));
#endif

  PRINT_TEST_PASS(__func__);
}

void testCopyAssign() {
  PRINT_FUNC_HEADER(__func__);
#if OVERLOAD_NEW_DELETE
  HEAP_BYTES_ALLOCATED = 0;
#endif
  // Part 1 -- Copy assign without heap allocation
  constexpr size_t staticSize = 4;
  auto myVec = SBOVectorWithInitSize<int, staticSize>();
  for (int i = 0; i < staticSize; i++) myVec.push_back(i);

  auto myVecCopy = SBOVectorWithInitSize<int, staticSize>();
  myVecCopy = myVec;

  for (int i = 0; i < staticSize; i++) {
    assert(myVec[i] == myVecCopy[i]);
  }

#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == 0);
#endif

  // Part 2 -- Copy assign with heap allocation, not enough existing capacity
  myVec.push_back(staticSize);
  myVecCopy = myVec;
  assert(myVecCopy.size() == myVec.size());
  assert(myVecCopy.capacity() == myVec.capacity());
  for (int i = 0; i < myVec.size(); i++) {
    assert(myVec[i] == myVecCopy[i]);
  }

#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == sizeof(int) * staticSize * 2 * 2);
#endif

  // Part 3 -- Copy assign with heap allocation, existing has enough capacity
  for (int i = 5; i < staticSize * 2 + 1; i++) myVecCopy.push_back(i);
  myVec.push_back(9);
  myVecCopy = myVec;
  assert(myVec.size() == myVecCopy.size());
  assert(myVecCopy.capacity() == myVec.capacity() * 2);
  for (int i = 0; i < myVec.size(); i++) {
    assert(myVec[i] == myVecCopy[i]);
  }

#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == sizeof(int) * staticSize * 2 * 2 + sizeof(int) * staticSize * 2 * 2);
#endif

  PRINT_TEST_PASS(__func__);
}

void testCopyConstructor() {
  PRINT_FUNC_HEADER(__func__);
#if OVERLOAD_NEW_DELETE
  HEAP_BYTES_ALLOCATED = 0;
#endif
  // Part 1 -- Copy constructor without heap allocation
  constexpr size_t staticSize = 4;
  auto myVec = SBOVectorWithInitSize<int, staticSize>();
  for (int i = 0; i < staticSize; i++) myVec.push_back(i);

  auto myVecCopy = SBOVectorWithInitSize<int, staticSize>(myVec);

  for (int i = 0; i < staticSize; i++) {
    assert(myVec[i] == myVecCopy[i]);
  }

#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == 0);
#endif

  // Part 2 -- Copy constructor with heap allocation
  for (int i = staticSize; i < staticSize * 2; i++) myVec.push_back(i);

#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == sizeof(int) * staticSize * 2);
#endif

  auto myVecCopy2 = SBOVectorWithInitSize<int, staticSize>(myVec);

#if OVERLOAD_NEW_DELETE
  assert(HEAP_BYTES_ALLOCATED == 2 * (sizeof(int) * staticSize * 2));
#endif

  for (int i = 0; i < myVec.size(); i++) {
    assert(myVec[i] == myVecCopy2[i]);
  }

  PRINT_TEST_PASS(__func__);
}
