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
	ThreadPool(size_t threads);

	template<typename F,typename... Args>
	auto enqueue(F&& f,Args&&... args)
		->std::future<typename std::result_of<F(Args...)>::type>;

	~ThreadPool();
	
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>>tasks;

	std::mutex queue_mutex;
	std::condition_variable condition;

	bool stop;

};

#endif