#pragma once
#include <queue>
#include <mutex>
#include <functional>
#include "Puzzle.h"

using namespace std;

class PPQueue
{

private:
	std::priority_queue<Puzzle,std::vector<Puzzle>, std::greater<std::vector<Puzzle>::value_type>> queue;

public:

	void push(Puzzle p) {
        queue.push(p);
	}

    Puzzle pop() {
        Puzzle p = queue.top();
        queue.pop();
        return p;
    }

    bool empty() {
        return queue.empty();
    }

    int size() {
        return queue.size();
    }
};

