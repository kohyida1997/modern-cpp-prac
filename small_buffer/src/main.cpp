#include <stdio.h>  // std::printf

#include <cassert>
#include <iostream>
#include <utility>  // std::pair

#include "shared/SBOVector.h"

/* Debug options */
#define OVERLOAD_NEW_DELETE 1

/* Helper Macros */
#define PRINT_FUNC_HEADER(a) std::cout << "\n+++ Inside: " << a << std::endl;
#define PRINT_FUNC_FINISH(b) std::cout << "+++ Finish: " << b << std::endl;
#define PRINT_MODULE_HEADER()                                                 \
  std::cout << "=== Welcome to the Small Buffer Optimization submodule! Get " \
               "ready to "                                                    \
               "have a small buffer! ===\n";

/* Overload global operator new and delete */
#if OVERLOAD_NEW_DELETE
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

/* Types */
using size_t = std::size_t;

template <typename T>
using SBOVector = MySBOContainers::SBOVector<T>;

template <typename T, size_t F>
using SBOVectorWithInitSize = MySBOContainers::SBOVector<T, F>;

/* Function prototypes */
void testStackAllocationOnly();
void testStackAllocationOverrideDefaultStaticCapacity();

// Main Driver code
int main() {
  // Say Hello Y'all
  PRINT_MODULE_HEADER();
  testStackAllocationOnly();
  testStackAllocationOverrideDefaultStaticCapacity();
  return 0;
}

/* Helper functions */
template <typename VectorType, typename Printer>
void printVectorRange(const VectorType& vec, size_t start, size_t endExclusive,
                      Printer print) {
  std::cout << ">>> vec(" << start << "," << endExclusive << "): [";
  for (; start < endExclusive; start++) {
    print(vec[start]);
    std::cout << ",";
    if (!(start + 1 >= endExclusive)) std::cout << " ";
  }
  std::cout << "]\n";
}
constexpr auto defaultPrinter = [](const auto& e) { std::cout << e; };
template <typename VectorType>
inline void printVectorRange(const VectorType& vec, size_t start,
                             size_t endExclusive) {
  printVectorRange(vec, start, endExclusive, defaultPrinter);
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
  for (st i = 0; i < count; i++) myVec.push_back(1.0 * i);
  printVectorRange(myVec, 0, count);

  // Push_back 10 more elements
  st more = 10;
  for (st i = 0; i < more; i++) myVec.push_back(1.0 * i + count + 0.1);
  printVectorRange(myVec, 0, more + count);

  try {
    myVec.push_back(0.2);
  } catch (std::runtime_error e) {
    std::cout << e.what() << std::endl;
  }
  assert(myVec.size() == more + count);
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

  try {
    v.push_back(std::make_pair(0, 0.0));
  } catch (std::runtime_error e) {
    std::cout << e.what() << std::endl;
  }
  assert(v.size() == count);
}
