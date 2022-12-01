#include <cmath>
namespace MyItoa {

char* itoaBase10(int v, char* buf) {
  char* ptr = buf;
  if (v == 0) {
    *buf = '0';
    *(buf + 1) = '\0';
    return buf;
  }
  bool adjust = false;
  if (v < 0) {
    // edge case
    if (v == -2147483648) {
      v++;
      adjust = true;
    }
    *ptr = '-';
    ptr++;
    v *= -1;
  }

  // get digits
  int digits = (int)(log10(v));

  // Set null terminator
  *(ptr + digits + 1) = '\0';

  // set last digit
  *(ptr + digits) = '0' + (v % 10) + adjust;
  v /= 10;
  digits--;

  // Do the rest
  while (v != 0) {
    *(ptr + digits) = '0' + (v % 10);
    v /= 10;
    digits--;
  }

  return buf;
}

}  // namespace MyItoa
