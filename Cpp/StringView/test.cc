#include "string_view.h"
#include <iostream>

int main()
{
    const char *data = "helloWorld 12345678";

    string_view sv1{data};
    string_view sv2{data, 5};
    string_view sv3{data + 3, 3};
    string_view sv4{data + 5};

    std::cout << data << std::endl;
    std::cout << sv1 << std::endl;
    std::cout << sv2 << std::endl;
    std::cout << sv3 << std::endl;
    std::cout << sv4 << std::endl;
}
