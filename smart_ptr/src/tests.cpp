#include <cassert>
#include <iostream>

#include "shared/SingleThreadSharedPtr.h"

#define PRINT_EXPECTS 0
// Helper Macros
#define PRINT_FUNC_HEADER(a) std::cout << "+++ Inside: " << a << std::endl;
#define PRINT_FUNC_FINISH(b) std::cout << "+++ Finish: " << b << std::endl;
#define PRINT_SHARED_PTR_IS(c) \
  std::printf("--- %s is [%p, %f]\n", #c, c.get(), *c);

#define PRINT_EXPECT_MSG(s) \
  if (PRINT_EXPECTS) std::cout << s;

#define PRINT_TEST_HEADER(c) std::cout << "=== Running Test " << c << " ===\n";

// Helpers
template <typename T>
using SingleThreadSharedPtr = MySmartPtrs::SingleThreadSharedPtr<T>;

template <typename T>
static void testFuncPassByValue(SingleThreadSharedPtr<T> ptr) {
  PRINT_FUNC_HEADER(__func__);
  ptr.printRefCountAndResource();
  PRINT_FUNC_FINISH(__func__);
}

namespace Tests {

// Test 1
void runTest1() {
  PRINT_TEST_HEADER(1);

  // Allocate some resource
  auto* x = new float(5.0);
  using resource_t = std::remove_pointer_t<decltype(x)>;

  PRINT_EXPECT_MSG("\n||| EXPECT: Only Heap allocation printed above ||| \n\n");

  // Create the shared pointer
  auto s_ptr1 = SingleThreadSharedPtr<resource_t>(x);
  s_ptr1.printRefCountAndResource();
  PRINT_SHARED_PTR_IS(s_ptr1);

  PRINT_EXPECT_MSG("\n||| EXPECT: Ref count 1, value is 5.0 ||| \n\n");

  // Create another shared pointer
  auto s_ptr_temp = SingleThreadSharedPtr<resource_t>(new resource_t(7.0));

  PRINT_EXPECT_MSG(
      "\n||| EXPECT: Heap allocation 8 and 4 bytes only above ||| \n\n");

  // Copy assign to ptr1
  s_ptr_temp = s_ptr1;

  PRINT_EXPECT_MSG(
      "\n||| EXPECT: Copy assign executed, resource and ref deletion "
      "for previous allocations ||| \n\n");

  // Pass it into a function (by value)
  testFuncPassByValue(s_ptr1);  // pass to testFunctionFoo here
  s_ptr1.printRefCountAndResource();

  PRINT_EXPECT_MSG(
      "\n||| EXPECT: Copy construct executed, RefCount=3 inside "
      "function, RefCount=2 outside function ||| \n\n");

  // Move assign the pointer
  s_ptr1 = SingleThreadSharedPtr<resource_t>(new resource_t(6.0));

  PRINT_EXPECT_MSG(
      "\n||| EXPECT: Move assign executed, transient resource's destructor "
      "is called, but resource already stolen so nothing deleted ||| \n\n");

  // Copy construct a new shared_ptr using the existing one
  auto s_ptr2 = SingleThreadSharedPtr<resource_t>(s_ptr1);

  PRINT_EXPECT_MSG(
      "\n||| EXPECT: Copy constructor executed, nothing printed ||| \n\n");

  PRINT_SHARED_PTR_IS(s_ptr1);
  PRINT_SHARED_PTR_IS(s_ptr2);

  assert(s_ptr1.get() == s_ptr2.get());
  s_ptr1.printRefCountAndResource();

  PRINT_EXPECT_MSG(
      "\n||| EXPECT: s_ptr1 refCount is 2, resource held by s_ptr2 as "
      "well. ||| \n\n");

  // Move construct to new pointer
  auto s_ptr3 = SingleThreadSharedPtr<resource_t>(std::move(s_ptr1));

  PRINT_EXPECT_MSG(
      "\n||| EXPECT: Move constructor executed, nothing printed ||| \n\n");

  assert(s_ptr1.get() == nullptr);
  s_ptr1.printRefCountAndResource();
  s_ptr2.printRefCountAndResource();
  s_ptr3.printRefCountAndResource();

  PRINT_EXPECT_MSG(
      "\n||| EXPECT: s_ptr2 and s_ptr3 refCount = 2, s_ptr1 is "
      "invalidated. ||| \n\n");
}

// Test 2: Edges cases involving invoking copy assign AFTER being
// invalidated
void runTest2() {
  PRINT_TEST_HEADER(2);
  using resource_t = int;
  auto* x = new resource_t(3);

  SingleThreadSharedPtr<resource_t> ptr0(x);
  assert(ptr0.hasResource());
  assert(ptr0.refCount() == 1);
  assert(ptr0.get() == x);

  // Edge case 1: Self copy assign
  ptr0 = ptr0;
  assert(ptr0.hasResource());
  assert(ptr0.refCount() == 1);
  assert(ptr0.get() == x);

  // Edge case 2: Resources invalidated, ptr0 stack object still exists
  SingleThreadSharedPtr<resource_t> ptr1(std::move(ptr0));
  assert(!ptr0.hasResource());
  assert(ptr1.hasResource());
  assert(ptr0.get() == nullptr);
  assert(ptr1.get() == x);
  assert(ptr1.refCount() == 1);

  // Edge case 3: ptr0 invoke Copy assign after previously moved
  ptr0 = ptr1;
  assert(ptr0.hasResource());
  // ptr0 and ptr1 both refer to the same thing
  assert(ptr0.refCount() == 2);
  assert(ptr0.get() == x);
  assert(ptr0.get() == ptr1.get());
}

// Test 3: Edge cases involving invoking move assign AFTER being invalidated
void runTest3() {
  PRINT_TEST_HEADER(3);
  using resource_t = std::string;
  auto* x = new resource_t(
      "6ac3c336e4094835293a3fed8a4b5fedde1b5e2626d9838fed50693bba00af0e");
  SingleThreadSharedPtr<resource_t> ptr0(x);
  assert(ptr0.hasResource());
  assert(ptr0.refCount() == 1);
  assert(ptr0.get() == x);

  // Edge case 1: Self move assign
  ptr0 = std::move(ptr0);
  assert(ptr0.hasResource());
  assert(ptr0.refCount() == 1);
  assert(ptr0.get() == x);

  // Edge case 2: Move construct ptr1 from ptr0, now ptr0 is invalid
  auto ptr1 = move(ptr0);
  assert(!ptr0.hasResource());
  assert(ptr1.hasResource());
  assert(ptr0.get() == nullptr);
  assert(ptr1.get() == x);
  assert(ptr1.refCount() == 1);

  SingleThreadSharedPtr<resource_t> ptr2(
      new std::string("1273yundj87y122312382183"));
  auto* y = ptr2.get();
  assert(ptr2.hasResource());
  assert(ptr2.refCount() == 1);
  assert(ptr2.get() == y);

  // Edge case 3: Ptr0 invokes Move assign after it was previously invalidated
  ptr0 = std::move(ptr2);
  assert(ptr0.hasResource());
  assert(ptr0.refCount() == 1);
  assert(ptr0.get() == y);
  assert(!ptr2.hasResource());
  assert(ptr2.get() == nullptr);

  // Edge case 4: Everyone now points back at object x, object y is no longer
  // owned
  ptr0 = ptr1;
  ptr2 = ptr1;
  // object at addr y should be cleaned up
  assert(ptr0.refCount() == ptr1.refCount() &&
         ptr1.refCount() == ptr2.refCount() && ptr2.refCount() == 3);
  assert(ptr0.get() == ptr1.get() && ptr1.get() == ptr2.get() &&
         ptr2.get() == x);
}

}  // namespace Tests
