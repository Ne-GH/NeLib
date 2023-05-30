/*******************************************************************************
 * Author : yongheng
 * Data   : 2023/05/30 20:20
*******************************************************************************/

#ifndef THREADPOOL_THREADPOOL_HPP
#define THREADPOOL_THREADPOOL_HPP
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <future>
#include <condition_variable>

class ThreadPool {
    std::vector<std::thread> _work;
    std::queue<std::function<void()>> _tasks; // 任务队列
    std::condition_variable _cv;
    std::mutex _mutex;

    bool stop = false;
    size_t _pool_size;
public:
    ThreadPool(size_t size) : _pool_size(size) {
        for (int i = 0; i < size; i++) {
            _work.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        //临界区开始 等待线程池的销毁或者任务
                        std::unique_lock<std::mutex> lock(_mutex);
                        _cv.wait(lock, [this] { return this->stop || !this->_tasks.empty(); }); //条件变量的wait函数 防止虚假唤醒

                        if (this->stop && this->_tasks.empty()) {
                            return; //线程池销毁 跳出循环
                        }

                        task = std::move(this->_tasks.front()); //将队列的头部移动给task
                        this->_tasks.pop(); //记得出队列
                    }
                    task(); //执行任务
                }
            });
        }
    }


    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            stop = true;
        }
        _cv.notify_all();   //通知所有的线程退出循环
        for_each(_work.begin(), _work.end(), std::mem_fn(&std::thread::join)); //等待
    }

    template<typename Func, typename ... Args>
    decltype(auto) Enqueue(Func&& func, Args&& ... args) {
        using RetType = typename std::invoke_result<Func,Args...>::type; //获取返回值类型

        auto task = std::make_shared< std::packaged_task<RetType()> >(
                std::bind(std::forward<Func>(func), std::forward<Args>(args)...) //通过bind和完美转发将函数和参数绑定
        );

        std::future<RetType> res = task->get_future();

        {
            //临界区开始 锁任务队列
            std::unique_lock<std::mutex> lock(_mutex);

            //如果已经将线程池析构,就不允许再入队列
            if (stop)
                throw std::runtime_error("线程池停止");

            _tasks.emplace([task]() { (*task)(); });
        }
        _cv.notify_one(); // 唤起一个线程
        return res;
    }
};






#endif //THREADPOOL_THREADPOOL_HPP
