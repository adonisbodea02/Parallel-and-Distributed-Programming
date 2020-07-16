// Lab7.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <math.h>
#include <future>
#define DIMENSION 128

using namespace std;

void parallel_adding_up(int* a, int i, int d, int power) {
	a[i + power - 1] += a[i + (power >> 1) - 1];
}

void parallel_adding_down(int* a, int i, int d, int power) {
	int t = a[i + (power >> 1) - 1];
	a[i + (power >> 1) - 1] = a[i + power - 1];
	a[i + int(pow(2, d + 1)) - 1] += t;
}


void prefix_sums(int* a) {
	//in place alg.

	int i;
	for (int d = 0; d < int(log2(DIMENSION)); d++)
	{
		i = 0;
		while(i < DIMENSION)
		{
			a[i + int(pow(2, d+1)) - 1] += a[i + int(pow(2, d)) - 1];
			i += int(pow(2, d + 1));
		}
	}

	a[DIMENSION - 1] = 0;

	int t;
	for (int d = int(log2(DIMENSION)) - 1; d >= 0; d--)
	{
		int i = 0;
		while (i < DIMENSION)
		{
			t = a[i + int(pow(2, d)) - 1];
			a[i + int(pow(2, d)) - 1] = a[i + int(pow(2, d + 1)) - 1];
			a[i + int(pow(2, d + 1)) - 1] += t;
			i += int(pow(2, d + 1));
		}
	}

}

void prefix_sums(int* a, int* b){
	//new list alg.

	int i;
	for (int d = 0; d < int(log2(DIMENSION)); d++)
	{
		i = 0;
		while (i < DIMENSION)
		{
			int power = int(pow(2, d + 1));
			auto ftr = async(parallel_adding_up, b, i, d, power);
			i += power;
		}
	}

	b[DIMENSION - 1] = 0;

	for (int d = int(log2(DIMENSION)) - 1; d >= 0; d--)
	{
		int i = 0;
		while (i < DIMENSION)
		{
			int power = int(pow(2, d + 1));
			auto ftr = async(parallel_adding_down, b, i, d, power);
			i += power;
		}
	}

	b[DIMENSION] = a[DIMENSION - 1] + b[DIMENSION - 1];
}

void print_list(int *a, int size) {
	cout << '\n';
	for (int i = 0; i < size; i++)
		cout << a[i] << " ";
	cout << '\n';
}

int main()
{
	int *a = new int[DIMENSION];
	int *b = new int[DIMENSION];
	for (int i = 0; i < DIMENSION; i++)
	{
		a[i] = i + 1;
		b[i] = i + 1;
	}
	print_list(a, DIMENSION);
	prefix_sums(a, b);
	print_list(b, DIMENSION + 1);
}
