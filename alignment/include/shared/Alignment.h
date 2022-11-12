#pragma once

namespace Alignment {

void sayHelloWorld() noexcept;

// Memory align for struct general rules:
// See:
// https://stackoverflow.com/questions/4306186/structure-padding-and-packing
// 1. Before each individual member, there will be padding so that to make it
//    start at an address that is divisible by its alignment requirement.
//
// 2. For struct, other than the alignment need for each individual member,
//    the size of whole struct itself will be aligned to a size divisible by
//    strictest alignment requirement of any of its members, by padding at end.
//

// Note: there isn't standardization at binary level for cpp. So the struct
//    padding and actual size might be different depending on compiler

// Possible layout
// P = padding
// X, Y and A are the bytes for the members.
// [X X X X | P P P P | Y Y Y Y Y Y Y Y | A A A A | P P P P]
// See: https://stackoverflow.com/questions/5397447/struct-padding-in-c
struct StructWithPadding {
  int x_;
  double y_;
  int a_;

  void printSize() noexcept;
};

// Possible layout
// P = padding
// X, Y and A are the bytes for the members.
// [Y Y Y Y Y Y Y Y | X X X X | A A A A]
struct StructWithoutPadding {
  double y_;
  int x_;
  int a_;

  void printSize() noexcept;
};

// Possible layout
// P = padding
// X, Y, C and J are the bytes for the members.
// [X X X X | P P P P | Y Y Y Y Y Y Y Y | J J J | C | P P P P]
struct StructWithCharArrayAndPadding {
  int x_;
  double y_;
  char j_[3];
  char c_;

  void printSize() noexcept;
};

}  // namespace Alignment