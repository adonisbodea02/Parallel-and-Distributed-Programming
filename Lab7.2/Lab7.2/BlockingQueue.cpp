#pragma once
#include "pch.h"
#include<queue>
#include<condition_variable>
#include<mutex>

template <typename T>
class BlockingQueue
{
private:
	std::queue<T> q;
	std::mutex mtx;
	std::condition_variable cv;

public:

	void push(T const value) {
		std::unique_lock<std::mutex> lck(mtx);
		q.push(value);
		cv.notify_one();
	}

	T pop() {
		std::unique_lock<std::mutex> lck(mtx);
		while (q.empty()) {
			cv.wait(lck);
		}
		T top = q.front();
		q.pop();
		return top;
	}

	int size() {
		return q.size();
	}
};

