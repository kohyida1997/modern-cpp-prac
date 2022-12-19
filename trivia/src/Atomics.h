#pragma once

#include <atomic>
#include <cassert>

/*
Example 1 -> Relaxed Memory ordering, reordering of load/store statements
Taken from:
https://www.justsoftwaresolutions.co.uk/threading/memory_models_and_synchronization.html
*/

namespace AtomicExample1 {

std::atomic<int> x(0);
std::atomic<int> y(0);

void thread1() {
  // Storing
  x.store(1, std::memory_order_relaxed);  // -- A
  y.store(1, std::memory_order_relaxed);  // -- B
}

void thread2() {
  // Loading
  int readY = y.load(std::memory_order_relaxed);  // -- C
  int readX = x.load(std::memory_order_relaxed);  // -- D

  if (readY == 1) assert(readX == 1);  // Assertion
}

/* Explanation

- The assertion is NOT guaranteed to pass.
- B and A can be reordered from the point of view of thread2.
- (B -> C -> D -> A) is entirely possible from thread2's POV. This means that
  the store of y=1 appears to be executed BEFORE store of x=1 from thread2.
- Thread2 loads readY to be 1, and readX to be 0, causing the assertion to fail.

From the article:
-> "Therefore, even though thread1 stores x before y, there is no guarantee that
    the writes will reach thread2 in that order"

*/

}  // namespace AtomicExample1

/*
Example 2 -> Relaxed Memory ordering, seemingly "out-of-order" behavior due to
re-ordering. Taken from:
https://www.justsoftwaresolutions.co.uk/threading/memory_models_and_synchronization.html
*/

namespace AtomicExample2 {
std::atomic<int> x(0);
std::atomic<int> y(0);

void thread1() {
  // Load Y
  int read_Y = y.load(std::memory_order_relaxed);  // -- A

  // Store what is loaded in Y into X
  x.store(read_Y, std::memory_order_relaxed);  // -- B
}

void thread2() {
  // Load X
  int read_X = x.load(std::memory_order_relaxed);  // -- C

  // Store "42" into Y
  y.store(42, std::memory_order_relaxed);  // -- D

  // Seems like it will always fail, but in fact can pass
  assert(read_X == 42);  // Assertion
}

/* Explanation

- The assertion that (read_X == 42) can actually be true
- There is no enforced ordering on the order in which thread1 sees C and D
- Thread1 could see D executed first, and read_Y = 42, then store 42 into X
- Thread2 would thus see read_X = 42 and fulfil the assertion.

*/

}  // namespace AtomicExample2

/*
Example 3 -> Default consistency model, sequential consistency. Strongest
guarantees. Taken from: https://gcc.gnu.org/wiki/Atomic/GCCMM/AtomicSync
*/

namespace AtomicExample3 {
std::atomic<int> x(0);
int y;

void thread1() {
  y = 1;       // -- A
  x.store(2);  // -- B
}

void thread2() {
  if (x.load() == 2) {  // -- C
    assert(y == 1);     // Assertion can never fail
  }
}

/* Explanation

- The default memory model synchronization mode is seq_cst aka. sequential
  consistency.
- The visibility of all operations across ALL threads will be sequentially
  consistent.
- They will be equivalent to some sequential history and will follow program
  order, even on non-atomic variables and unrelated variables.
- A <happens-before> B is enforced
- If Thread2 sees B, then it MUST see A due to transitivity of <happens-before>
- The optimizer CANNOT reorder A <> B as the <happens-before> relationship MUST
  be preserved.

From the article:
-> "The store to 'y' happens-before the store to x in thread 1. If the load of
    'x' in thread 2 gets the results of the store that happened in thread 1, it
    must all see all operations that happened before the store in thread 1, even
    unrelated ones."

*/

}  // namespace AtomicExample3
