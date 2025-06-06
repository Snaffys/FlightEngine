#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back([this] { this->worker(); });
}

void ThreadPool::worker() {
    for (;;) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(this->queueMutex);
            this->condition.wait(lock, [this] { return this->stop || !this->jobs.empty(); });
            if (this->stop && this->jobs.empty())
                return;
            job = std::move(this->jobs.front());
            this->jobs.pop();
        }
        job();
    }
}

void ThreadPool::enqueue(std::function<void()> job) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobs.emplace(std::move(job));
    }
    condition.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
}
