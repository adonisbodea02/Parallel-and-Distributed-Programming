#include<mutex>
#include<condition_variable>
#include<vector>
#include <functional>
class ThreadPool {

private:
	std::mutex mutex;
	std::condition_variable cond;
	std::condition_variable condEnd;
	std::vector<std::function<void()> > queue;
	bool end;
	unsigned int liveThreads;
	std::vector<std::thread> threads;
public:
	ThreadPool(unsigned int nrThreads);
	~ThreadPool();
	void close();
	void enqueue(std::function<void()> func);

private:
	void run();
};