#include <shared/EndianChecker.h>
#include <shared/MyItoa.h>
#include <stdint.h>

#include <cassert>
#include <iostream>
#include <string>

#define PRINT_FUNC_HEADER(a) std::cout << "\n+++ Inside: " << a << std::endl;

void sayHello() { std::cout << "=== Hello from Trivia Y'all ===\n"; }

void checkEndian();
void itoaTest();

int main() {
  // Say hi
  sayHello();
  checkEndian();
  itoaTest();
  return 0;
}

void itoaTest() {
  PRINT_FUNC_HEADER(__func__);
  char buf[17];
  int test = 0;
  assert(std::to_string(test) == std::string(MyItoa::itoaBase10(test, buf)));

  test = 1;
  assert(std::to_string(test) == std::string(MyItoa::itoaBase10(test, buf)));

  test = -1;
  assert(std::to_string(test) == std::string(MyItoa::itoaBase10(test, buf)));

  test = INT32_MAX;
  assert(std::to_string(test) == std::string(MyItoa::itoaBase10(test, buf)));

  test = INT32_MIN;
  assert(std::to_string(test) == std::string(MyItoa::itoaBase10(test, buf)));

  std::cout << "Done testing my custom itoa" << std::endl;
}

void checkEndian() {
  PRINT_FUNC_HEADER(__func__);
  bool little = EndianChecker::isLittleEndian();
  std::cout << "The system is ";
  std::cout << (little ? "Little Endian" : "Big Endian") << '!' << std::endl;
}

/* Does not execute anything meaningful, just for reading sake. More to do with
 * the compiler */
void constInfo() {
  // This is a (const char) pointer
  const char* c = "HelloWorld";
  c = "GoodByeWorld";  // === This is OK
  // c[0] = 'T';    // === This is NOT OK

  // This is a const (char) pointer
  char temp[] = "Orange\0";
  char* const q = temp;
  // q = c;   // === This is NOT OK
  q[0] = 'A';  // === This is OK

  // This is a const (const char) pointer
  const char* const p = "GoodByeWorld";
  // p = c;        // === This is NOT OK
  // p[0] = 'B';   // === This is NOT OK
}
