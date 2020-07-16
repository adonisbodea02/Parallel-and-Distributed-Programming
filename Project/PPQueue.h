#pragma once
#include <queue>
#include <mutex>
#include <functional>
#include "Puzzle.h"

using namespace std;

/*
Syncronized priority queue container for Puzzles
*/
class PPQueue
{

private:
	//priority queue container for the puzzles which uses the heurstic as a comparator for ordering
	std::priority_queue<Puzzle,std::vector<Puzzle>, std::greater<std::vector<Puzzle>::value_type>> queue;
	//mutex used for the syncronizing of the operations
	std::mutex mtx;

public:

	void push(Puzzle p) {
        std::unique_lock<std::mutex> lkc(mtx);
        queue.push(p);
	}

    Puzzle pop() {
        std::unique_lock<std::mutex> lkc(mtx);
        Puzzle p = queue.top();
        queue.pop();
        return p;
    }

    bool empty() {
        std::unique_lock<std::mutex> lkc(mtx);
        return queue.empty();
    }
};

