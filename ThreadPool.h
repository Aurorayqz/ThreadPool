// 
// ThreadPool.hpp
// ThreadPool

#ifndef ThreadPool_hpp
#define ThreadPool_hpp 
#include <vector>               // std::vector
#include <queue>                // std::queue
#include <memory>               // std::make_shared
#include <stdexcept>            // std::runtime_error
#include <thread>               // std::thread
#include <mutex>                // std::mutex,        std::unique_lock
#include <condition_variable>   // std::condition_variable
#include <future>               // std::future,       std::packaged_task
#include <functional>           // std::function,     std::bind
#include <utility>              // std::move,         std::forward

class ThreadPool
{
public:
	inline ThreadPool(size_t threads):stop(false){
		// 启动 threads 数量的工作线程(worker)
		for (size_t i = 0; i < threads; ++i)
		{
			workers.emplace_back(
				// 此处的 lambda 表达式捕获 this, 即线程池实例
				[this]
				{
					for(;;)
					{
						std::function<void()>task;

						// 临界区
						{
							std::unique_lock<std::mutex> lock(this->queue_mutex);
							this->condition.wait(lock,[this]{
								return this->stop||!this->tasks.empty();
							});
							if (this->stop&&this->tasks.empty())
							{
								return ;
							}
							task=std::move(this->tasks.front());
							this->tasks.pop();
						}
						task();
					}
				}
				);
		}
	}

	// 销毁所有线程池中创建的线程
	inline ~ThreadPool(){
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}

		condition.notify_all();

		for (std::thread &worker:workers)
		{
			worker.join();
		}
	}

	template<typename F,typename... Args>
	auto enqueue(F&& f,Args&&... args)
		->std::future<typename std::result_of<F(Args...)>::type>
		{
			using return_type = typename std::result_of<F(Args...)>::type;

			auto task = std::make_shared<std::packaged_task<return_type()> >(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...));

			std::future<return_type> res = task->get_future();

			// 临界区
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				if (stop)
				{
					throw std::runtime_error("enqueue on stopped ThreadPool");
				}
				tasks.emplace([task]{(*task)();});
			}

			condition.notify_one();
			return res;

		}

	
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>>tasks;

	std::mutex queue_mutex;
	std::condition_variable condition;

	bool stop;

};

#endif