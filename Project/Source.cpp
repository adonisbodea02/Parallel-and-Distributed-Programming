#include<iostream>
#include<functional>
#include<mutex>
#include<Windows.h>
#include "Puzzle.h"
#include "Pair.h"
#include "ThreadPool.h"
#include "PPQueue.h"


using namespace std;

#define NRT 4

//mutex used for writing the best solution
mutex mtx;
//puzzle which retains the best solution found so far in terms of number of moves made
Puzzle bestSolution;
//thread pool used for launching parallel searches
ThreadPool tp(NRT);
bool found=false;
//priority queue used for storing different puzzle configuration to be explored
PPQueue q;

//function which computes the best solution for a certain puzzle
void search(Puzzle p) {

	//if there exists a solution and the number of move for this configuration is bigger than the numbers of moves of the best solution we exit
	if (found && p.getMoves() >= bestSolution.getMoves())
		return;

	int sol = p.isSolution();
	if (sol != -1) {
		mtx.lock();
		//if there is no solution found or a better solution is found we update the result
		if (!found || sol < bestSolution.getMoves()) {
			bestSolution = p;
			found = true;
			mtx.unlock();
			return;
		}
		mtx.unlock();
	}
	else {
		//we explore other configurations by adding the possible moves from this one
		vector<Pair> moves = p.getPossibleMoves();
		for (Pair move : moves) {
			Puzzle movedPuzzle(p);
			movedPuzzle.makeMove(move);
			q.push(movedPuzzle);
		}

		//we first check the "best" configurations according to the heuristic
		for (Pair move : moves) {
			Puzzle toSearch = q.pop();
			tp.enqueue([toSearch]() {search(toSearch); });
		}
	}
}


int main() {

	Puzzle p;
	p.shuffle(20);
	p.printPuzzle();
	cout << '\n';

	search(p);

	tp.close();

	bestSolution.printPuzzle();
	cout << bestSolution.getMoves()<<'\n';
	
	return 0;
}