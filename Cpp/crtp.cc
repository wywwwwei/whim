#include <iostream>

/**
* Curiously Recurring Template Pattern
* 1. Compile-time Polymorphism. More efficient than runtime polymorphism (e.g., using virtual functions)
* 2. Compile-time Introspection. 
*/

using std::cout;
using std::endl;

template <typename Derived> class CBase {
  public:
    void commonMethod() {
        static_cast<Derived *>(this)->implementation();
    }
};

class Derived1 : public CBase<Derived1> {
  public:
    void implementation() {
        cout << "Implementation in Derived1" << endl;
    }
};

class Derived2 : public CBase<Derived2> {
  public:
    void implementation() {
        cout << "Implementation in Derived2" << endl;
    }
};

void test_CRTP() {
    Derived1 d1;
    d1.commonMethod();
    d1.implementation();

    Derived2 d2;
    d2.commonMethod();
    d2.implementation();
}

int main() {
    test_CRTP();
}