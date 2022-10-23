#include "shared/Alignment.h"
#include <iostream>

int main() {
    // Say hi
    Alignment::sayHelloWorld();

    (new Alignment::StructWithoutPadding())->printSize();
    (new Alignment::StructWithPadding())->printSize();

    std::cout << sizeof(double) << std::endl;

    return 0;
}