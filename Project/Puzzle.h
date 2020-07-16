#pragma once

#include<random>
#include<array>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <iomanip> 
#include <stdlib.h>
#include "Pair.h"


/*
Class used for representing the puzzle object of the problem
*/
class Puzzle
{
private:
	//the matrix which keeps the state of the puzzle
	int puzzle[4][4] = { 0 };
	//the coordinates of the position of the free space denoted by 0
	Pair freeSpace;
	//how many moves were done so far in order for the puzzle to be solved
	int moves;
public:
	
	
	/*
	Constructor for Puzzle Class - the initial configuration is that of a solved puzzle which can be shuffled
	*/
	Puzzle() {
		std::array<int, 16> nrs{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };

		int k = 0;
		for(int i=0;i<4;i++)
			for (int j = 0; j < 4; j++) {
				if (nrs[k] == 0) {
					freeSpace = Pair(i, j);
				}
				puzzle[i][j] = nrs[k++];
				
			}
		moves = 0;
	}

	/*
	Functions which returns possible moves from a certain configuration of the puzzle
	@return vector<Pair> - vector with pairs (line and column) denoting the available positions in which the empty tile can be moved
	*/
	std::vector<Pair> getPossibleMoves() {
		std::vector<Pair> moves;
		if (freeSpace.line == 0) {
			if (freeSpace.col == 0) {
				moves.emplace_back(freeSpace.line + 1,freeSpace.col);
				moves.emplace_back(freeSpace.line, freeSpace.col + 1);
			}
			else if (freeSpace.col == 3) {
				moves.emplace_back(freeSpace.line + 1, freeSpace.col);
				moves.emplace_back(freeSpace.line, freeSpace.col - 1);
			}
			else {
				moves.emplace_back(freeSpace.line + 1, freeSpace.col);
				moves.emplace_back(freeSpace.line, freeSpace.col + 1);
				moves.emplace_back(freeSpace.line, freeSpace.col - 1);
			}
		}
		else if (freeSpace.line == 3) {
			if (freeSpace.col == 0) {
				moves.emplace_back(freeSpace.line - 1, freeSpace.col);
				moves.emplace_back(freeSpace.line, freeSpace.col + 1);
			}
			else if (freeSpace.col == 3) {
				moves.emplace_back(freeSpace.line - 1, freeSpace.col);
				moves.emplace_back(freeSpace.line, freeSpace.col - 1);
			}
			else {
				moves.emplace_back(freeSpace.line - 1, freeSpace.col);
				moves.emplace_back(freeSpace.line, freeSpace.col + 1);
				moves.emplace_back(freeSpace.line, freeSpace.col - 1);
			}
		}
		else if (freeSpace.col == 0) {
			moves.emplace_back(freeSpace.line - 1, freeSpace.col);
			moves.emplace_back(freeSpace.line + 1, freeSpace.col);
			moves.emplace_back(freeSpace.line, freeSpace.col + 1);
		}
		else if (freeSpace.col == 3) {
			moves.emplace_back(freeSpace.line - 1, freeSpace.col);
			moves.emplace_back(freeSpace.line + 1, freeSpace.col);
			moves.emplace_back(freeSpace.line, freeSpace.col - 1);
		}
		else {
			moves.emplace_back(freeSpace.line - 1, freeSpace.col);
			moves.emplace_back(freeSpace.line + 1, freeSpace.col);
			moves.emplace_back(freeSpace.line, freeSpace.col - 1);
			moves.emplace_back(freeSpace.line, freeSpace.col + 1);
		}

		int seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(moves.begin(), moves.end(), std::default_random_engine(seed));
		return moves;
	}

	/*
	Function which makes a move on the puzzle (swaps the 0 tile with with the element in the specified position by the Pair)
	The move is counted for the total of moves made to solve the puzzle
	@param Pair toMove - the tile to be moved in the empty place
	*/
	void makeMove(Pair toMove) {
		int aux = puzzle[freeSpace.line][freeSpace.col];
		puzzle[freeSpace.line][freeSpace.col] = puzzle[toMove.line][toMove.col];
		puzzle[toMove.line][toMove.col] = aux;

		freeSpace = toMove;
		moves++;
	}

	/*
	Function which returns the coordinates of the free space
	@return Pair - the coordinates of the free space
	*/
	Pair getFreeSpace() {
		return freeSpace;
	}

	/*
	Fuction which verifies if this configuration of the puzzle is the solution.
	@return int - the number of moves made to arrive at the solution or -1 if it is not the solution.
	*/
	int isSolution() {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++) {
				if (puzzle[i][j] != i * 4 + j)
					return -1;
			}
		return moves;
	}

	/*
	Function which returns the numbers of moves made from the initial configuration
	@return int
	*/
	int getMoves() {
		return moves;
	}

	/*
	Heuristic for calculating how far from the solution the existing configuration is (Manhattan distance is used)
	@return int
	*/
	int getHeuristic() const {
		int sum = 0;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++) {
				sum += std::abs(i * 4 + j - puzzle[i][j]);
			}

		return sum;
	}

	/*
	Function which prints the puzzle in the form of a matrix
	*/
	void printPuzzle() {
		
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				std::cout << std::setw(3);
				std::cout << puzzle[i][j] << " ";
			}
			std::cout << std::setw(2);
			std::cout << '\n';
		}
	}

	/*
	Functions which shuffles the puzzle (makes an indicated number of moves from the solution configuration)
	@param int shuffles - the numbers of random moves to make from the solution configuration
	*/
	void shuffle(int shuffles) {
		std::vector<Pair> moves;
		for (int i = 0; i < shuffles; i++) {
			makeMove(getPossibleMoves()[0]);
		}
		this->moves -= shuffles;
	}

	friend bool operator < (Puzzle const& lhs, Puzzle const& rhs) {
		return lhs.getHeuristic() < rhs.getHeuristic();
	}
	friend bool operator > (const Puzzle& lhs, const Puzzle& rhs) {
		return lhs.getHeuristic() > rhs.getHeuristic();
	}	

	~Puzzle() {

	}
	
};

