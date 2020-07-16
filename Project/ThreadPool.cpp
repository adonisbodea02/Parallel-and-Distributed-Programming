#include "ThreadPool.h"
#include <thread>

ThreadPool::ThreadPool(unsigned int nrThreads)
{
	this->liveThreads = nrThreads;
	this->end = false;
	threads.reserve(nrThreads);
	for (unsigned int i = 0; i < nrThreads; i++)
	{
		threads.emplace_back([this]() {this->run(); });
	}
}

ThreadPool::~ThreadPool()
{
	this->close();
	for (std::thread& t :threads) {
		t.join();
	}
}

void ThreadPool::close()
{
	std::unique_lock<std::mutex> lck(mutex);
	end = true;
	cond.notify_all();
	while (liveThreads > 0) {
		condEnd.wait(lck);
	}
}

void ThreadPool::enqueue(std::function<void()> func)
{
	std::unique_lock<std::mutex> lck(mutex);
	queue.push_back(std::move(func));
	cond.notify_one();
}

void ThreadPool::run()
{
	while (true) {
		std::function<void()> toExec;
		{
			std::unique_lock<std::mutex> lck(mutex);
			while (queue.empty() && !end) {
				cond.wait(lck);
			}
			if (queue.empty()) {
				liveThreads--;
				if (!liveThreads) {
					condEnd.notify_all();
				}
				return;
			}
			toExec = std::move(queue.front());
			queue.erase(queue.begin());
		}
		toExec();
	}
}
