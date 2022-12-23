#include <bits/stdc++.h>

#include <cassert>
#include <iostream>

using namespace std;

/*

vector push_back vs. emplace_back behaviour.
TLDR:
    - push_back() tries to move construct if possible, then default to copy
construct. (in-place)
    - emplace_back() directly let's you access the constructors of the
underlying class
*/

namespace PushVsEmplace {

class Item {
 private:
  char _c;

 public:
  // Constructor
  Item() : _c('?') {
    cout << this << " => ";
    cout << "Default Constructor called\n";
  }

  // Parameterized constructor
  Item(char c) : _c(c) {
    cout << this << " => ";
    cout << "Parameterized constructor called\n";
  }

  // Copy Constructor
  Item(const Item& other) : _c(other._c) {
    cout << this << " => ";
    cout << "Copy constructor called\n";
  }

  // Move constructor
  Item(Item&& other) : _c(other._c) {
    cout << this << " => ";
    cout << "Move constructor called\n";
    other._c = '\0';  // invalidate
  }

  // Destructor
  ~Item() {
    cout << this << " => ";
    cout << "Destructor called\n";
  }

  // Debug
  void printItem() const { cout << "Item[\'" << _c << "\']" << endl; }

  char get() const { return _c; }
};

void runExamples() {
  vector<Item> vec;
  // Avoid realloc
  vec.reserve(16);

  // Case 1: push_back with lvalue
  cout << "Case 1:" << endl;
  Item one('1');       // parametrized constructor
  vec.push_back(one);  // in-place copy-construct
  cout << endl;

  // Case 2: push_back with rvalue taken from previous lvalue
  cout << "Case 2:" << endl;
  Item two('2');                  // parametrized constructor
  vec.push_back(std::move(two));  // in-place move-construct
  // two must be invalidated
  assert(two.get() == '\0');
  cout << endl;

  // Case 3: push_back with rvalue directly in arg
  cout << "Case 3:" << endl;
  vec.push_back(
      Item('3'));  // in-place move construct + destructor for temporary
  cout << endl;

  // Case 4: emplace_back with copy
  cout << "Case 4:" << endl;
  Item four('4');          // parametrized constructor
  vec.emplace_back(four);  // in-place copy construct
  cout << endl;

  // Case 5: emplace_back with args
  cout << "Case 5:" << endl;
  vec.emplace_back(
      '5');  // directly construct using parameterized constructor, no move/copy
  cout << endl;

  // Case 6: emplace_back with rvalue of previous lvalue
  cout << "Case 6:" << endl;
  Item six('6');                     // parametrized constructor
  vec.emplace_back(std::move(six));  // in-place move-construct
  // two must be invalidated
  assert(six.get() == '\0');
  cout << endl;

  // Case 7: emplace_back with temporary rvalue
  cout << "Case 7:" << endl;
  vec.emplace_back(
      Item('7'));  // in-place move construct + destructor for temporary
  cout << endl;

  // Case 8: emplace_back with default constructor
  cout << "Case 8:" << endl;
  vec.emplace_back();  // default constructor
  cout << endl;

  // Debug
  cout << "\nPrinting Vector: " << endl;
  for_each(vec.begin(), vec.end(), [&vec](const auto& i) {
    cout << &i - &(*vec.begin()) << ": ";
    i.printItem();
  });
  cout << endl;

  /*

  Behaviour when resizing: since C++11, vector will try to use move constructor
  if it exists. See:
  https://stackoverflow.com/questions/27136883/will-copy-constructor-of-an-object-be-called-upon-resize-of-a-vector

  */

  return;
}
}  // namespace PushVsEmplace
