#include "pch.h"
#include <iostream>

#include<iostream>
#include<vector>
#include"ThreadPool.h"
#include<future>

#define DIMENSION 500

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
	for (int counter = 0; counter < DIMENSION; counter++) {
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

void launch_no_of_lines_threads_addition(int **a, int **b, int **c) {
	ThreadPool t(4);

	for (int i = 0; i < DIMENSION; i++) {
		t.enqueue([i, a, b, c]() {add_matrices_by_lines(a[i], b[i], c[i]); });
	}

	t.close();
}

void launch_no_of_elements_threads_addition(int **a, int **b, int **c) {

	ThreadPool t(4);

	for (int i = 0; i < DIMENSION; i++) {
		for (int j = 0; j < DIMENSION; j++)
			t.enqueue([a, b, i, j, c]() {add_matrices_by_elements(a, b, i, j, c); });
	}

	t.close();
}

void launch_no_of_lines_threads_multiplication(int **a, int **b, int **c) {

	ThreadPool t(4);

	for (int i = 0; i < DIMENSION; i++) {
		t.enqueue([i, a, b, c]() {multiply_matrices_by_lines(a[i], b, c[i]); });
	}

	t.close();
}

void launch_no_of_elements_threads_multiplication(int **a, int **b, int **c) {
	ThreadPool t(4);

	for (int i = 0; i < DIMENSION; i++) {
		for (int j = 0; j < DIMENSION; j++)
			t.enqueue([a, b, i, j, c]() {multiply_matrices_by_elements(a[i], b, i, j, c); });
	}

	t.close();
}

void launch_no_of_lines_futures_addition(int **a, int **b, int **c) {
	vector<future<void>> futures;
	for (int i = 0; i < DIMENSION; i++) {
		futures.push_back(async(add_matrices_by_lines, a[i], b[i], c[i]));
	}
	for (unsigned int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
}
void launch_no_of_elements_futures_addition(int **a, int **b, int **c) {
	vector<future<void>> futures;
	for (int i = 0; i < DIMENSION; i++) {
		for (int j = 0; j < DIMENSION; j++) {
			futures.push_back(async(add_matrices_by_elements, a, b, i, j, c));
		}
	}
	for (unsigned int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
}

void launch_no_of_lines_futures_multiplication(int **a, int **b, int **c) {

	vector<future<void>> futures;
	for (int i = 0; i < DIMENSION; i++) {
		futures.push_back(async(multiply_matrices_by_lines, a[i], b, c[i]));
	}
	for (unsigned int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
}

void launch_no_of_elements_futures_multiplication(int **a, int **b, int **c) {
	vector<future<void>> futures;
	for (int i = 0; i < DIMENSION; i++) {
		for (int j = 0; j < DIMENSION; j++) {
			futures.push_back(async(multiply_matrices_by_elements, a[i], b, i, j, c));
		}
	}
	for (unsigned int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
}

void menu() {

	cout << "0. Exit\n";
	cout << "1. Addition with one thread\n";
	cout << "2. Addition with N threads - Thread Pool\n";
	cout << "3. Addition with N*N threads - Thread Pool\n";
	cout << "4. Multiplication with one thread\n";
	cout << "5. Multiplication with N threads - Thread Pool\n";
	cout << "6. Multiplication with N*N threads - Thread Pool\n";
	cout << "7. Addition with N futures\n";
	cout << "8. Addition with N*N futures\n";
	cout << "9. Multiplication with N futures\n";
	cout << "10. Multiplication with N*N futures\n";
	cout << "Enter command: ";
}

int main()
{
	std::chrono::steady_clock::time_point begin, end;
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

	int command;

	while (true) {
		menu();
		cin >> command;
		switch (command) {
		case 0:
			return 0;

		case 1:
			begin = chrono::high_resolution_clock::now();
			classic_addition(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		case 2:
			begin = chrono::high_resolution_clock::now();
			launch_no_of_lines_threads_addition(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		case 3:
			begin = chrono::high_resolution_clock::now();
			launch_no_of_elements_threads_addition(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		case 4:
			begin = chrono::high_resolution_clock::now();
			classic_multiplication(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		case 5:
			
			begin = chrono::high_resolution_clock::now();
			launch_no_of_lines_threads_multiplication(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		case 6:
			
			begin = chrono::high_resolution_clock::now();
			launch_no_of_elements_threads_multiplication(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		case 7:

			begin = chrono::high_resolution_clock::now();
			launch_no_of_lines_futures_addition(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		case 8:

			begin = chrono::high_resolution_clock::now();
			launch_no_of_elements_futures_addition(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		case 9:

			begin = chrono::high_resolution_clock::now();
			launch_no_of_lines_futures_multiplication(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		case 10:

			begin = chrono::high_resolution_clock::now();
			launch_no_of_elements_futures_multiplication(a, b, c);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			break;

		default:
			cout << "No such option\n";
		}
	}

	return 0;
}
