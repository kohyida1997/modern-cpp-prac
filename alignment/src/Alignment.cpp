#include "shared/Alignment.h"

#include <iostream>

namespace Alignment {

void sayHelloWorld() noexcept {
  std::cout << "Alignment Exercise - Hello World!" << std::endl;
}

void StructWithPadding::printSize() noexcept {
  std::cout << "Size of StructWithPadding is " << sizeof(StructWithPadding)
            << std::endl;
}

void StructWithoutPadding::printSize() noexcept {
  std::cout << "Size of StructWithoutPadding is "
            << sizeof(StructWithoutPadding) << std::endl;
}

void StructWithCharArrayAndPadding::printSize() noexcept {
  std::cout << "Size of StructWithCharArrayAndPadding is "
            << sizeof(StructWithCharArrayAndPadding) << std::endl;
}

}  // namespace Alignment