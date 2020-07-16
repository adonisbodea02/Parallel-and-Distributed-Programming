#pragma once

#include <iostream>
class Pair
{
public:
	int line;
	int col;

	Pair() {
		line = 0;
		col = 0;
	}

	Pair(int l,int c) {
		line = l;
		col = c;
	}


	void printPair() {
		std::cout <<"Pair: "<< line << " " << col << "\n";
	}
};

