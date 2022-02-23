// "Pointer to implementation" or "pImpl" is a C++ programming technique
// that removes implementation details of a class from its object representation
// by placing them in a separate class, accessed through an opaque pointer

// reference linking
// https://docs.microsoft.com/en-us/cpp/cpp/pimpl-for-compile-time-encapsulation-modern-cpp?view=msvc-170
// https://en.cppreference.com/w/cpp/language/pimpl

#include <memory>

class Interface
{
public:
    // all public and protected stuff goes here
    Interface();
    ~Interface();
    
    void doSomething();
private:
    // forward declaration of the implementation class
    class Implementation;
    std::unique_ptr<Implementation> pImpl_;
};