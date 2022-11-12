#include <iostream>

#include "shared/Alignment.h"

int main() {
  // Say hi
  Alignment::sayHelloWorld();

  (new Alignment::StructWithoutPadding())->printSize();
  (new Alignment::StructWithPadding())->printSize();
  (new Alignment::StructWithCharArrayAndPadding())->printSize();

  // On gcc 11.3.0, and Intel x86-64 Intel(R) Xeon(R) Platinum 8259CL CPU
  // @ 2.50GHz the alignment of the structs are same as alignment of the
  // strictest member (aka. the double with an align of 8)
  std::cout << "Align of StructWithoutPadding is "
            << alignof(Alignment::StructWithoutPadding) << std::endl;
  std::cout << "Align of StructWithPadding is "
            << alignof(Alignment::StructWithPadding) << std::endl;
  std::cout << "Align of StructWithCharArrayAndPadding is "
            << alignof(Alignment::StructWithCharArrayAndPadding) << std::endl;

  return 0;
}