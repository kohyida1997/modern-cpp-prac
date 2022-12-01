#include <iostream>
namespace EndianChecker {

bool isLittleEndian() {
  // In Hex: 0x 41 00 00 42
  int test = 'B' + ('A' << 3 * 8);

  // On little endian, we expect
  // the least significant byte to
  // be stored at the front ie.
  // [42 00 00 00 41]

  auto* charPtr = reinterpret_cast<char*>(&test);
  std::cout << "Checking Endianness...\n";
  std::cout << "First byte = " << *charPtr << '\n';
  std::cout << "Last byte = " << *(charPtr + 3) << std::endl;
  return *charPtr == 'B';
}

}  // namespace EndianChecker