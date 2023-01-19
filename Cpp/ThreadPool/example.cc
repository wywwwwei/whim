#include "ThreadPool.h"
#include <random>

std::random_device rv;
std::mt19937 generator(rv());
std::uniform_int_distribution<std::mt19937::result_type> custom_random(-1000, 1000);

void run(int arg)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2000 + custom_random(generator)));
    printf("%d\n", arg * 10);
}

int main()
{
    ThreadPool pool(5);
    for (int i = 0; i < 10; ++i)
    {
        pool.submit(run, i);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    // std::this_thread::sleep_for(std::chrono::milliseconds(4000));
}