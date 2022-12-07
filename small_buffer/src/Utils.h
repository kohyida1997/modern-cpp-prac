#pragma once
#include <iostream>

namespace Utils {
/* Helper Macros */
#define PRINT_FUNC_HEADER(a) std::cout << "\n+++ Test: " << a << std::endl;
#define PRINT_MODULE_HEADER()                                                 \
  std::cout << "=== Welcome to the Small Buffer Optimization submodule! Get " \
               "ready to "                                                    \
               "have a small buffer! ===\n";
#define PRINT_TEST_PASS(f) std::cout << "✓✓✓ PASSED: " << f << std::endl;

/* Helper functions */
template <typename VectorType, typename Printer>
void printVectorRange(const VectorType& vec, size_t start, size_t endExclusive,
                      Printer print) {
  endExclusive = std::min(endExclusive, vec.size());
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

template <typename VectorType>
inline void printVector(const VectorType& vec) {
  printVectorRange(vec, 0, vec.size());
}

}  // namespace Utils
