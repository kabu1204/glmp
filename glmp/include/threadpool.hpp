//
// Created by 于承业 on 2022/4/9.
//

#ifndef GLMP_THREADPOOL_HPP
#define GLMP_THREADPOOL_HPP

#include "vector"
#include "queue"
#include "thread"
#include "future"
#include "mutex"
#include "memory"
#include "atomic"
#include "type_traits"
#include "functional"
#include "condition_variable"
#include "concurrentqueue.hpp"
#define THREADPOOL_MAX_TASKS_SIZE ~(1<<31)-1

/**
 * std::thread && 无锁队列实现的线程池
 */
class threadpool{
public:
    /**
     * @brief 创建一个线程池
     * @param n_thread 创建的worker线程数量。默认为1。
     * @param sleep_interval 任务队列为空时，线程睡眠时间
     */
    explicit threadpool(uint32_t n_threads=std::thread::hardware_concurrency(), uint32_t sleep_interval=35)
    : _n_threads(n_threads), _sleep_interval(sleep_interval), status(0)
    {
        for(int i=0;i<n_threads;++i){
            workers.emplace_back([this]{
                while(status>0){
                    if(tasks.empty()){
                        std::this_thread::sleep_for(std::chrono::milliseconds(_sleep_interval));
                        continue;
                    }
                    std::function<void()> task = tasks.front();
                    tasks.pop();
                    task();
                }
            });
        }
    }
    /**
     * @brief 添加任务，如果线程池已停止，则会抛出失败的错误
     * @tparam F std::function<void()>
     * @tparam Args 参数类型
     * @param function 函数
     * @param args 参入函数的参数
     * @return 所提交任务的future对象，可以获取任务返回值
     */
    template<typename F, typename... Args>
    auto submit(F&& function, Args&&...args){
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(function), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        if(status<0) throw std::runtime_error("threadpool stopped");
        tasks.emplace([task](){ (*task)(); });
        return res;
    }
    /**
     * @brief 停止线程池，禁止提交任务并且回收线程
     */
    void stop(){
        status=-1;
        for(std::thread& th:workers) th.join();
    }
private:
    uint32_t _n_threads;
    uint32_t _sleep_interval;
    std::atomic<uint8_t> status;
    std::vector<std::thread> workers;
    LinkedConcurrentQueue<std::function<void()>> tasks;
};
#endif //GLMP_THREADPOOL_HPP
