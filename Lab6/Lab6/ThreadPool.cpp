#pragma once
#include "pch.h"
#include <stdio.h>
#include <stdint.h>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <functional>
#include <condition_variable>
#include <list>

class ThreadPool {
public:
	explicit ThreadPool(size_t nrThreads)
		:m_end(false),
		m_liveThreads(nrThreads)
	{
		m_threads.reserve(nrThreads);
		for (size_t i = 0; i < nrThreads; ++i) {
			m_threads.emplace_back([this]() {this->run(); });
		}
	}

	~ThreadPool() {
		close();
		for (std::thread& t : m_threads) {
			t.join();
		}
	}

	void close() {
		std::unique_lock<std::mutex> lck(m_mutex);
		m_end = true;
		m_cond.notify_all();
		while (m_liveThreads > 0) {
			m_condEnd.wait(lck);
		}
	}

	void enqueue(std::function<void()> func) {
		std::unique_lock<std::mutex> lck(m_mutex);
		m_queue.push_back(std::move(func));
		m_cond.notify_one();
	}

	//    template<typename Func, typename... Args>
	//    void enqueue(Func func, Args&&... args) {
	//        std::function<void()> f = [=](){func(args...);};
	//        enqueue(std::move(f));
	//    }
private:
	void run() {
		while (true) {
			std::function<void()> toExec;
			{
				std::unique_lock<std::mutex> lck(m_mutex);
				while (m_queue.empty() && !m_end) {
					m_cond.wait(lck);
				}
				if (m_queue.empty()) {
					--m_liveThreads;
					if (0 == m_liveThreads) {
						m_condEnd.notify_all();
					}
					return;
				}
				toExec = std::move(m_queue.front());
				m_queue.pop_front();
			}
			toExec();
		}
	}

	std::mutex m_mutex;
	std::condition_variable m_cond;
	std::condition_variable m_condEnd;
	std::list<std::function<void()> > m_queue;
	bool m_end;
	size_t m_liveThreads;
	std::vector<std::thread> m_threads;
};
