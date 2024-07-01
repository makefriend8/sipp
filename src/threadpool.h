#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include <future>
#include <stdexcept>

// 基础任务类，所有任务类都继承自该类，并实现 run 方法
class Task {
public:
    virtual void run() = 0;
    virtual ~Task() = default;
};

// 线程池类
class ThreadPool {
public:
    ThreadPool(size_t);
    ~ThreadPool();

    std::future<void> enqueue(std::shared_ptr<Task> task);

private:
    // 工作线程
    std::vector<std::thread> workers;
    // 任务队列
    std::queue<std::pair<std::shared_ptr<Task>, std::shared_ptr<std::promise<void>>>> tasks;

    // 同步
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};


// 一个示例任务类，继承自 Task 类并实现 run 方法
class ExampleTask : public Task {
public:
    ExampleTask(int id) : id(id) {}
    void run() override {
        //std::cout << "Task " << id << " is running." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 模拟任务执行时间
    }
private:
    int id;
};
