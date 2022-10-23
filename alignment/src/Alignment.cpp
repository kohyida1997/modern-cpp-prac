#include "shared/Alignment.h"

#include <iostream>

namespace Alignment {

void sayHelloWorld() noexcept {
  std::cout << "Alignment Exercise - Hello World!" << std::endl;
}

void OneCacheLiner::print() noexcept { std::cout << "Hello from Alignment\n"; }

void StructWithPadding::printSize() noexcept {std::cout << "Size of StructWithPadding is " << sizeof(StructWithPadding) << std::endl;}

void StructWithoutPadding::printSize() noexcept {std::cout << "Size of StructWithoutPadding is " << sizeof(StructWithoutPadding) << std::endl;}

}  // namespace Alignment