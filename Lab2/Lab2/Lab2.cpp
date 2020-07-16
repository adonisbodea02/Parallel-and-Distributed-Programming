#include "pch.h"
#include <iostream>
#include <ctime>
#include <vector>
#include <thread>

#define DIMENSION 1000

using namespace std;

void print_matrix(int **a)
{
	for (int i = 0; i < DIMENSION; i++)
	{
		for (int j = 0; j < DIMENSION; j++)
			cout << a[i][j] << " ";
		cout << '\n';
	}
}

void classic_addition(int **a, int **b, int **c) {

	for (int i = 0; i < DIMENSION; i++)
		for (int j = 0; j < DIMENSION; j++)
			c[i][j] = a[i][j] + b[i][j];
}

void add_matrices_by_elements(int **a, int **b, int i, int j, int **c) {
	c[i][j] = a[i][j] + b[i][j];
}

void add_matrices_by_lines(int *a, int *b, int *c) {
	
	for (int i = 0; i < DIMENSION; i++)
		c[i] = a[i] + b[i];
}

void classic_multiplication(int **a, int **b, int **c) {
	for (int i = 0; i < DIMENSION; i++)
		for (int j = 0; j < DIMENSION; j++)
		{
			c[i][j] = 0;
			for (int k = 0; k < DIMENSION; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
}

void multiply_matrices_by_elements(int *a, int **b, int i, int j, int **c) {
	c[i][j] = 0;
	for (int counter = 0; counter < DIMENSION; counter++){
			c[i][j] += a[counter] * b[counter][j];
	}
}

void multiply_matrices_by_lines(int *a, int **b, int *c) {
	for (int i = 0; i < DIMENSION; i++) {
		c[i] = 0;
		for (int j = 0; j < DIMENSION; j++)
			c[i] += a[j] * b[j][i];
	}
}


int main()
{
	int** a = new int*[DIMENSION];
	for (int i = 0; i < DIMENSION; ++i)
		a[i] = new int[DIMENSION];

	int** b = new int*[DIMENSION];
	for (int i = 0; i < DIMENSION; ++i)
		b[i] = new int[DIMENSION];

	int** c = new int*[DIMENSION];
	for (int i = 0; i < DIMENSION; ++i)
		c[i] = new int[DIMENSION];

	for (int i = 0; i < DIMENSION; i++)
		for (int j = 0; j < DIMENSION; j++)
		{
			a[i][j] = i+j;
			b[i][j] = i+j;
		}

	std::vector<std::thread*> threads;

	chrono::steady_clock::time_point start = chrono::steady_clock::now();

	/*
	addition
	*/

	
	/*threads.push_back(new std::thread(&classic_addition, a, b, c));
	for (int ithrd = 0; ithrd < 1; ithrd++) {
		threads[ithrd]->join();
	}*/

	
	for (int i = 0; i < DIMENSION; i++) {
		threads.push_back(new std::thread(&add_matrices_by_lines, a[i], b[i], c[i]));
	}

	for (int ithrd = 0; ithrd < DIMENSION; ithrd++) {
		threads[ithrd]->join();
	}
	

	/*for (int i = 0; i < DIMENSION; i++) {
		for(int j = 0; j < DIMENSION; j++)
			threads.push_back(new std::thread(&add_matrices_by_elements, a, b, i, j, c));
	}

	for (int iThrd = 0; iThrd < DIMENSION * DIMENSION; iThrd++)
		threads[iThrd]->join();*/
	


	/*
	multiplication*/

	/*threads.push_back(new std::thread(&classic_multiplication, a, b, c));
	for (int ithrd = 0; ithrd < 1; ithrd++) {
		threads[ithrd]->join();
	}*/
	
	/*for (int i = 0; i < DIMENSION; i++) {
		threads.push_back(new std::thread(&multiply_matrices_by_lines, a[i], b, c[i]));
	}

	for (int ithrd = 0; ithrd < DIMENSION; ithrd++) {
		threads[ithrd]->join();
	}*/
	
	/*for (int i = 0; i < DIMENSION; i++) {
		for (int j = 0; j < DIMENSION; j++)
			threads.push_back(new std::thread(&multiply_matrices_by_elements, a[i], b, i, j, c));
	}

	for (int iThrd = 0; iThrd < DIMENSION * DIMENSION; iThrd++)
		threads[iThrd]->join();*/

	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << "BENCHMARK: \nRunning took "
		<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
		<< "ms.\n";

	return 0;
}