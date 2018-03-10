# ThreadPool
该项目是使用 C++ 实现的一个简单线程池。

## 项目介绍
- 多线程技术主要是解决单个处理器单元内多个线程的执行问题，由此诞生了所谓的线程池技术。线程池主要由三个基本部分组成：

- 线程池管理器（Thread Pool）：负责创建、管理线程池，最基本的操作为：创建线程池、销毁线程池、增加新的线程任务；
- 工作线程（Worker）：线程池中的线程，在没有任务时会处于等待状态，可以循环执行任务；
- 任务队列（Tasks Queue）：未处理任务的缓存队列。

## 项目相关
### C++11 标准库特性
- std::thread
- std::mutex, std::unique_lock
- std::condition_variable
- std::future, std::packaged_task
- std::function, std::bind
- std::shared_ptr, std::make_shared
- std::move, std::forward

### C++11 语言特性
- Lambda 表达式
- 尾置返回类型


## 运行结果（结果是随机的）
![test](https://github.com/Aurorayqz/ThreadPool/blob/master/ThreadPool.png)
- 首先四个 hello <i=0,1,2,3> 被输出，这是符合预期的，因为最初的线程池中的线程全部空闲，并且能够容纳四个线程同时执行；
