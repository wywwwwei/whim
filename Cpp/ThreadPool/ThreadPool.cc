#include "ThreadPool.h"

ThreadPool::ThreadWorker::ThreadWorker(ThreadPool *pool, const int id) : pool_(pool), id_(id)
{
}

void ThreadPool::ThreadWorker::operator()()
{
    while (!pool_->shutdown_)
    {
        std::unique_lock<std::mutex> lock(pool_->tasks_mutex_);

        if (pool_->tasks_queue_.empty())
        {
            pool_->conditional_variable_.wait(lock, [&]() { return pool_->shutdown_ || !pool_->tasks_queue_.empty(); });
        }

        // when shutdown, not continue exec task
        if (pool_->shutdown_)
        {
            break;
        }

        Task task = std::move(pool_->tasks_queue_.front());
        pool_->tasks_queue_.pop();

        lock.unlock();

        task();
    }
}

ThreadPool::ThreadPool(const int n_threads) : shutdown_(false)
{
    threads_.reserve(n_threads);
    for (int i = 0; i < n_threads; ++i)
    {
        threads_.emplace_back(ThreadWorker(this, i));
    }
}

ThreadPool::~ThreadPool()
{
    // when shutdown, throw remain tasks
    {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        while (!tasks_queue_.empty())
        {
            tasks_queue_.pop();
        }
    }

    shutdown_ = true;
    conditional_variable_.notify_all();
    for (auto &thread : threads_)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}