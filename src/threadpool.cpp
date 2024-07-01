#include "threadpool.h"
// 构造函数
ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for(size_t i = 0; i < threads; ++i) {
        workers.emplace_back(
            [this] {
                for(;;) {
                    std::pair<std::shared_ptr<Task>, std::shared_ptr<std::promise<void>>> task_pair;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task_pair = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task_pair.first->run();
                    task_pair.second->set_value(); // 设置任务完成
                }
            }
        );
    }
}

// 任务入队
std::future<void> ThreadPool::enqueue(std::shared_ptr<Task> task) {
    auto promise = std::make_shared<std::promise<void>>();
    auto future = promise->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace(task, promise);
    }
    condition.notify_one();
    return future;
}

// 析构函数
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}
