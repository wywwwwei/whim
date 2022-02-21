#include <stddef.h>
#include <array>

// Multidimensional Array
template <typename T, size_t R, size_t... C>
struct Matrix
{
    using ElementType = typename Matrix<T, C...>::Type;
    using Type = std::array<ElementType, R>;
};

template <typename T, size_t R>
struct Matrix<T, R>
{
    using Type = std::array<T, R>;
};

void matrix_test()
{
    constexpr size_t a = 2;
    constexpr size_t b = 3;
    constexpr size_t c = 4;
    constexpr size_t d = 5;
    Matrix<int, a, b, c, d>::Type matrix;
    int count = 0;
    for (int i = 0; i < a; ++i)
    {
        for (int j = 0; j < b; ++j)
        {
            for (int k = 0; k < c; ++k)
            {
                for (int l = 0; l < d; ++l)
                {
                    matrix[i][j][k][l] = count++;
                }
            }
        }
    }
}

int main()
{
    matrix_test();
}