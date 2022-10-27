#include "shared/BasicAllocator.h"
#include "shared/BasicArena.h"
#include <iostream>

// Override global new and delete
void* operator new(size_t size) {
  void* ptr = malloc(size);
    std::cout << "Heap Alloc: " << size << " bytes at " << ptr << "\n";
  return ptr;
}

void operator delete(void* ptr) { std::cout << "Deleted " << ptr << std::endl; }


int main() {
    // Say hi
    BasicAllocator::printBasicAllocatorHeader();

    BasicArena::BasicArena<1024>();

    return 0;
}