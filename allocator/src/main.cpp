#include <iostream>
#include <memory>

#include "shared/BasicAllocator.h"
#include "shared/BasicArena.h"

// Override global new and delete
void* operator new(size_t size) {
  void* ptr = malloc(size);
  std::cout << "Heap Alloc: " << size << " bytes at " << ptr << "\n";
  return ptr;
}

void* operator new(std::size_t count, std::align_val_t al) {
  void* ptr = malloc(count);
  std::cout << "Heap Alloc: " << count << " bytes at " << ptr
            << " with align [" << static_cast<size_t>(al)  
            << "]\n";
  return ptr;
}


void operator delete(void* ptr) { std::cout << "Deleted " << ptr << std::endl; }

int main() {
  // Say hi
  BasicAllocator::printBasicAllocatorHeader();

  BasicArena::BasicArena<1024>();

  return 0;
}