#pragma once

#include <atomic>
#include <cassert>
#include <vector>
#include <string>

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

/*
Example 4 -> Release Acquire model. Synchronize only between release/acquire on
the same atomic variable. Taken from:
https://en.cppreference.com/w/cpp/atomic/memory_order
*/

}  // namespace AtomicExample3

namespace AtomicExample4 {
std::atomic<std::string*> ptr;
int data;

void producer() {
  std::string* p = new std::string("Hello");
  data = 42;
  ptr.store(p, std::memory_order_release);
}

void consumer() {
  std::string* p2;
  while (!(p2 = ptr.load(std::memory_order_acquire)))
    ;
  assert(*p2 == "Hello");  // never fires
  assert(data == 42);      // never fires
}
}  // namespace AtomicExample4

/*
Case Study 1: Rigtorp's Lock Free SPSC (Not SPMC) Circular Buffer
Taken from: https://rigtorp.se/ringbuffer/
Also regarding power of two:
https://stackoverflow.com/questions/10527581/why-must-a-ring-buffer-size-be-a-power-of-2
*/

namespace RigTorpLockfreeCircularBuffer {
struct ringbuffer {
  std::vector<int> data_;
  // L1 Cache Line Aligned
  alignas(64) std::atomic<size_t> readIdx_{0};
  alignas(64) std::atomic<size_t> writeIdx_{0};

  ringbuffer(size_t capacity) : data_(capacity, 0) {}

  // nextWriteIdx == readIdx is used to signify buffer is full
  // eg. [X,_,X,X]
  //      ^ ^
  //      W R

  bool push(int val) {
    auto const writeIdx = writeIdx_.load(std::memory_order_relaxed);
    auto nextWriteIdx = writeIdx + 1;
    if (nextWriteIdx == data_.size()) {
      nextWriteIdx = 0;
    }
    if (nextWriteIdx == readIdx_.load(std::memory_order_acquire)) {
      return false;
    }
    data_[writeIdx] = val;
    writeIdx_.store(nextWriteIdx, std::memory_order_release);
    return true;
  }

  bool pop(int& val) {
    // Relaxed ordering for readIdx because only 1 consumer thread
    // and reader thread doesn't need to synchronize with itself
    auto const readIdx = readIdx_.load(std::memory_order_relaxed);

    // Use acquire on writeIdx to synchronize with producer thread,
    // ensure all operations BEFORE the release is seen.
    if (readIdx == writeIdx_.load(std::memory_order_acquire)) {
      return false;
    }

    // modify
    val = data_[readIdx];

    auto nextReadIdx = readIdx + 1;
    if (nextReadIdx == data_.size()) {
      nextReadIdx = 0;
    }

    // Use release to ensure producer thread sees my changes
    readIdx_.store(nextReadIdx, std::memory_order_release);
    return true;
  }
};
}  // namespace RigTorpLockfreeCircularBuffer

/* Explanation

- Note that (p2 = ptr.load(...)) is atomic.
- If the consumer goes past the while check, then it must have
  seen the store on the atomic ptr.
- A happens-before is established.
- All writes BEFORE the ptr.store will be visible, and the compiler
  will not re-order beyond the atomic_store_release.

*/

/*

Memory Order Modes:
Details: https://en.cppreference.com/w/cpp/atomic/memory_order

1. Sequential Consistency

- Instructions, both atomic or not can be interleaved.
- There exists a globally sequentially consistent execution that preserves
  program order.
- Re-ordering can happen between atomic operations, but not across the atomic
  operations
- All threads will observe the SAME sequence of modifications on the memory
  location in the same order.
- There is a global ordering across ALL atomic variables that is visible to
  everyone, but the actual order might not be known till runtime


2. Relaxed

- Only guarantee atomicity and modification order consistency
- No happens-before between unrelated atomic or non-atomic operations
- If thread1 sees a value of a variable X on thread2, it cannot later see older
  values of X from thread2. (Same variable)
- Can re-order non-dependent variables (dependent is if they access same memory
  location)
- Preserves modification order (RR, RW, WR, WW) on the SAME atomic variable only

3. Release-Acquire ordering

- Only possibly enforce happens-before between loads/stores of the SAME atomic
  variable across threads.
- If thread A store is tagged "release", and if thread B load is tagged
  "acquire", AND if thread B actually loads the value that thread A stored, then
  a happens-before relationship is guaranteed.
- Notice the importance of the last conditions. All memory writes, both
  non-atomic and relaxed atomic that <happened-before> the atomic store on
  thread A will be visible to thread B ONLY if B returns the value that thread A
  stored, or a value from later in the release sequence of thread A.
- This synchronization is only established between threads releasing/acquiring
  the same atomic variable. Other threads can see different order of memory
  access than what the synchronized threads see.
- A good example are MUTEXES or Atomic Spinlocks. After one thread RELEASES the
  lock after the critical section, another thread that acquires the lock is
  guaranteed to see the changes before the release of the lock in the critical
  section.

*/
