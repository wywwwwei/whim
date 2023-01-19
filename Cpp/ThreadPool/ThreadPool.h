#include <functional>
#include <future>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
  public:
    using Task = std::function<void()>;

  private:
    class ThreadWorker
    {
      public:
        ThreadWorker(ThreadPool *pool, const int id);
        void operator()();

      private:
        int id_;
        ThreadPool *pool_;
    };

  public:
    ThreadPool(const int n_threads = 0);

    // non-copyable
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    ~ThreadPool();

    // add a function to be executed, along with any arguments for it
    template <typename Func, typename... Args>
    auto submit(Func &&func, Args &&...args) -> std::future<decltype(func(args...))>;

  private:
    bool shutdown_;
    std::vector<std::thread> threads_;

    std::queue<Task> tasks_queue_;
    std::mutex tasks_mutex_;

    std::condition_variable conditional_variable_;
};

template <typename Func, typename... Args>
auto ThreadPool::submit(Func &&func, Args &&...args) -> std::future<decltype(func(args...))>
{
    using PackagedTask = std::packaged_task<decltype(func(args...))()>;
    auto packaged_task =
        std::make_shared<PackagedTask>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

    {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        tasks_queue_.emplace([packaged_task]() { (*packaged_task)(); });
    }

    conditional_variable_.notify_one();

    return packaged_task->get_future();
}