#include <stdio.h>

#include <cassert>
#include <iostream>
#include <type_traits>

#include "shared/SingleThreadSharedPtr.h"

// Helper Macros
#define PRINT_FUNC_HEADER(a) std::cout << "+++ Inside: " << a << std::endl;
#define PRINT_FUNC_FINISH(b) std::cout << "+++ Finish: " << b << std::endl;
#define PRINT_SHARED_PTR_IS(c) \
  std::printf("--- %s is [%p, %f]\n", #c, c.get(), *c);
#define PRINT_MODULE_HEADER() std::cout << "=== Welcome to the Smart Pointer submodule. Get ready to feel smart! ===\n";

// Overload global operator new and delete
void* operator new(size_t s) {
  auto ptr = malloc(s);
  std::cout << "New: Heap Allocated " << s << " bytes at " << ptr << std::endl;
  return ptr;
}

void operator delete(void* ptr) {
  free(ptr);
  std::cout << "Delete: Free " << ptr << std::endl;
}

// Helpers
template <typename T>
using SingleThreadSharedPtr = MySmartPtrs::SingleThreadSharedPtr<T>;

template <typename T>
void testFuncPassByValue(SingleThreadSharedPtr<T> ptr) {
  PRINT_FUNC_HEADER(__func__);
  ptr.printRefCountAndResource();
  PRINT_FUNC_FINISH(__func__);
}

// Main Driver code
int main() {
  // Say Hello Y'all
  PRINT_MODULE_HEADER();

  // Allocate some resource
  double* x = new double(5.0);
  using resource_type = std::remove_pointer_t<decltype(x)>;

  std::cout << "\n||| EXPECT: Only Heap allocation printed above ||| \n\n";

  // Create the shared pointer
  auto s_ptr1 = SingleThreadSharedPtr<resource_type>(x);
  s_ptr1.printRefCountAndResource();
  PRINT_SHARED_PTR_IS(s_ptr1);

  std::cout << "\n||| EXPECT: Ref count 1, value is 5.0 ||| \n\n";

  // Create another shared pointer
  auto s_ptr_temp = SingleThreadSharedPtr<resource_type>(new double(7.0));

  std::cout
      << "\n||| EXPECT: Heap allocation 8 and 4 bytes only above ||| \n\n";

  // Copy assign to ptr1
  s_ptr_temp = s_ptr1;

  std::cout << "\n||| EXPECT: Copy assign executed, resource and ref deletion "
               "for previous allocations ||| \n\n";

  // Pass it into a function (by value)
  testFuncPassByValue(s_ptr1);  // pass to testFunctionFoo here
  s_ptr1.printRefCountAndResource();

  std::cout << "\n||| EXPECT: Copy construct executed, RefCount=3 inside "
               "function, RefCount=2 outside function ||| \n\n";

  // Move assign the pointer
  s_ptr1 = SingleThreadSharedPtr<resource_type>(new double(6.0));

  std::cout
      << "\n||| EXPECT: Move assign executed, transient resource's destructor "
         "is called, but resource already stolen so nothing deleted ||| \n\n";

  // Copy construct a new shared_ptr using the existing one
  auto s_ptr2 = SingleThreadSharedPtr<resource_type>(s_ptr1);

  std::cout
      << "\n||| EXPECT: Copy constructor executed, nothing printed ||| \n\n";

  PRINT_SHARED_PTR_IS(s_ptr1);
  PRINT_SHARED_PTR_IS(s_ptr2);

  assert(s_ptr1.get() == s_ptr2.get());
  s_ptr1.printRefCountAndResource();

  std::cout << "\n||| EXPECT: s_ptr1 refCount is 2, resource held by s_ptr2 as "
               "well. ||| \n\n";

  // Move construct to new pointer
  auto s_ptr3 = SingleThreadSharedPtr<resource_type>(std::move(s_ptr1));

  std::cout
      << "\n||| EXPECT: Move constructor executed, nothing printed ||| \n\n";

  assert(s_ptr1.get() == nullptr);
  s_ptr1.printRefCountAndResource();
  s_ptr2.printRefCountAndResource();
  s_ptr3.printRefCountAndResource();

  std::cout << "\n||| EXPECT: s_ptr2 and s_ptr3 refCount = 2, s_ptr1 is "
               "invalidated. ||| \n\n";

  return 0;
}
