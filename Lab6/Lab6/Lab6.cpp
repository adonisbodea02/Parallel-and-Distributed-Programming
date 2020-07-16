#pragma once
#include "pch.h"
#include <cstdio>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include "ThreadPool.cpp"

#define DIMENSION 64

using namespace std;

void print_polynomial(long long *poly, unsigned int size)
{
	for (int i = 0; i < size; i++)
	{
		cout << poly[i];
		if (i != 0)
			cout << "x^" << i;
		if (i != size - 1)
			cout << " + ";
	}
}

long long *classic_multiplication_polynomials(long long *a, long long *b, unsigned int size)
{
	long long *prod = new long long[2*size];

	// Initialize the product polynomial 
	for (int i = 0; i < 2*size; i++)
		prod[i] = 0;

	// Multiply two polynomials term by term 

	// Take ever term of first polynomial 
	for (int i = 0; i < size; i++)
	{
		// Multiply the current term of first polynomial 
		// with every term of second polynomial. 
		for (int j = 0; j < size; j++)
			prod[i + j] += a[i] * b[j];
	}

	return prod;
}

long long *karatsuba_polynomial_multiplication(long long *a, long long *b, int posA, int posB, unsigned int size) {
	
	if (size <= 1)
	{
		long long *res = new long long[size];
		res[1] = 0;
		res[0] = a[posA] * b[posB];
		return res;
	}

	/*
	Let A = Ah * x^(N / 2) + Al
	and B = Bh * x^(N / 2) + Bl

	X = Al * Bl
	Y = Ah * Bh
	Z = (Al + Ah) * (Bl + Bh)

	A * B  = (Ah * x^(N / 2) + Al) * (Bh * x ^ (N / 2) + Bl)
		   = Al * Bl + Al * Bh * x^(N / 2) + Ah * Bl * x^(N / 2) + Ah*Bh*x^(N / 2)
	       = Al * Bl + (Al*Bh + Ah*Bl) * x^(N / 2) + Ah * Bh * x^(N / 2)
		   = X + (Z - Y - X) * X^(N / 2) + Y * x^(N / 2)
	*/

	long long *x = karatsuba_polynomial_multiplication(a, b, posA, posB, size/2);
	long long *y = karatsuba_polynomial_multiplication(a, b, posA + size/2, posB + size/2, size/2);
	long long* AhAl = new long long[size/2];
	long long* BhBl = new long long[size/2];

	for (int i = 0; i < size/2; i++) {
		AhAl[i] = 0;
		BhBl[i] = 0;
	}
	for (int i = 0; i < size/2; i++) {
		AhAl[i] = a[posA + i] + a[posA + i + (size/2)];
		BhBl[i] = b[posB + i] + b[posB + i + (size/2)];
	}

	long long *z = karatsuba_polynomial_multiplication(AhAl, BhBl, 0, 0, size/2);

	long long *c = new long long[size*2];

	for (int i = 0; i < size*2; i++) {
		c[i] = 0;
	}
	
	for (int i = 0; i < size; i++) {
		c[i] += x[i];
		c[size / 2 + i] += z[i];
		c[size / 2 + i] -= x[i];
		c[size / 2 + i] -= y[i];
		c[i + size] = y[i];
	}

	return c;
}

void parallelized_multiplication_polynomials(long long *a, long long *b, long long *c, unsigned int startA, unsigned int stopA, vector<mutex*> mtxs)
{
	for (int i = startA; i < stopA; i++)
	{
		for (int j = 0; j < DIMENSION; j++)
		{
			mtxs[i + j]->lock();
			c[i + j] += a[i] * b[j];
			mtxs[i + j]->unlock();
		}
	}
}

long long *parallelized_karatsuba_polynomial_multiplication(long long *a, long long *b, int posA, int posB, unsigned int size) {

	if (size <= 1)
	{
		long long *res = new long long[size];
		res[1] = 0;
		res[0] = a[posA] * b[posB];
		return res;
	}

	/*
	Let A = Ah * x^(N / 2) + Al
	and B = Bh * x^(N / 2) + Bl

	X = Al * Bl
	Y = Ah * Bh
	Z = (Al + Ah) * (Bl + Bh)

	A * B  = (Ah * x^(N / 2) + Al) * (Bh * x ^ (N / 2) + Bl)
		   = Al * Bl + Al * Bh * x^(N / 2) + Ah * Bl * x^(N / 2) + Ah*Bh*x^(N / 2)
		   = Al * Bl + (Al*Bh + Ah*Bl) * x^(N / 2) + Ah * Bh * x^(N / 2)
		   = X + (Z - Y - X) * X^(N / 2) + Y * x^(N / 2)
	*/
	
	auto futureX = async(karatsuba_polynomial_multiplication, a, b, posA, posB, size / 2);
	auto futureY = async(karatsuba_polynomial_multiplication, a, b, posA + size / 2, posB + size / 2, size / 2);
	long long* AhAl = new long long[size / 2];
	long long* BhBl = new long long[size / 2];

	for (int i = 0; i < size / 2; i++) {
		AhAl[i] = 0;
		BhBl[i] = 0;
	}
	for (int i = 0; i < size / 2; i++) {
		AhAl[i] = a[posA + i] + a[posA + i + (size / 2)];
		BhBl[i] = b[posB + i] + b[posB + i + (size / 2)];
	}

	auto futureZ = async(karatsuba_polynomial_multiplication, AhAl, BhBl, 0, 0, size / 2);

	long long *x = futureX.get();
	long long *y = futureY.get();
	long long *z = futureZ.get();

	long long *c = new long long[size * 2];

	for (int i = 0; i < size * 2; i++) {
		c[i] = 0;
	}

	for (int i = 0; i < size; i++) {
		c[i] += x[i];
		c[size / 2 + i] += z[i];
		c[size / 2 + i] -= x[i];
		c[size / 2 + i] -= y[i];
		c[i + size] = y[i];
	}

	return c;
}

long long* launch_classic_multiplication_polynomials(long long *a, long long *b) {
	return classic_multiplication_polynomials(a, b, DIMENSION);
}

long long* launch_karatsuba_multiplication_polynomials(long long *a, long long *b) {
	return karatsuba_polynomial_multiplication(a, b, 0, 0, DIMENSION);
}

void launch_parallelized_classic_multiplication_polynomials(long long *a, long long *b, long long *c, vector<mutex*> mtxs, unsigned int no_threads) {
	ThreadPool t(4);
	unsigned int share = DIMENSION / no_threads;
	int i = 0;
	for (i = 0; i < no_threads - 1; i++) {
		unsigned int start = share * i;
		unsigned int stop = share * (i + 1);
		t.enqueue([a, b, c, start, stop, mtxs]() {parallelized_multiplication_polynomials(a, b, c, start, stop, mtxs); });
	}
	int last_start = share * (no_threads - 1);
	int last_stop = DIMENSION;
	t.enqueue([a, b, c, last_start, last_stop, mtxs]() {parallelized_multiplication_polynomials(a, b, c, last_start, last_stop, mtxs); });
	t.close();
}

long long* launch_parallelized_karatsuba_multiplication_polynomials(long long *a, long long *b) {
	return parallelized_karatsuba_polynomial_multiplication(a, b, 0, 0, DIMENSION);
}

void menu() {
	cout << "0. Exit\n";
	cout << "1. Classic multiplication - sequential\n";
	cout << "2. Classic multiplication - parallelized\n";
	cout << "3. Karatsuba multiplication - sequential\n";
	cout << "4. Karatsuba multiplication - parallelized\n";
	cout << "Enter command: ";
}

int main()
{
	chrono::steady_clock::time_point begin, end;
	long long *a = new long long[DIMENSION];
	long long *b = new long long[DIMENSION];
	for (int i = 0; i < DIMENSION; i++)
	{
		a[i] = i+1;
		b[i] = i+1;
	}

	long long *c = new long long[DIMENSION * 2];
	for (int i = 0; i < DIMENSION * 2; i++)
		c[i] = 0;

	vector<mutex*> mtxs(2 * DIMENSION);
	for (int i = 0; i < 2 * DIMENSION; ++i)
		mtxs[i] = new std::mutex();

	long long *d;

	int command;

	while (true) {
		menu();
		cin >> command;
		switch (command) {
		case 0:
			return 0;

		case 1:
			begin = chrono::high_resolution_clock::now();
			d = launch_classic_multiplication_polynomials(a, b);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			print_polynomial(d, DIMENSION * 2 - 1);
			cout << '\n';
			break;

		case 2:
			unsigned int no_threads;
			cout << "Give number of threads: ";
			cin >> no_threads;
			begin = chrono::high_resolution_clock::now();
			launch_parallelized_classic_multiplication_polynomials(a, b, c, mtxs, no_threads);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			//print_polynomial(c, DIMENSION * 2 - 1);
			cout << '\n';
			break;

		case 3:
			begin = chrono::high_resolution_clock::now();
			d = launch_karatsuba_multiplication_polynomials(a, b);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			//print_polynomial(d, DIMENSION * 2 - 1);
			cout << '\n';
			break;

		case 4:
			begin = chrono::high_resolution_clock::now();
			d = launch_parallelized_karatsuba_multiplication_polynomials(a, b);
			end = chrono::high_resolution_clock::now();
			cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
			//print_polynomial(d, DIMENSION * 2 - 1);
			cout << '\n';
			break;

		default:
			cout << "No such option\n";
		}
	}
}