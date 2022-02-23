#include "pImpl.h"

class Interface::Implementation // defined privately here
{
    // all private data and functions:
    // all of these can now change without recompiling callers
public:
    void doSomething() {

    }

    void config() {
        
    }
};

Interface::Interface() : pImpl_(new Implementation())
{
    pImpl_->config();
}
Interface::~Interface() = default;

void Interface::doSomething()
{
    pImpl_->doSomething();
}