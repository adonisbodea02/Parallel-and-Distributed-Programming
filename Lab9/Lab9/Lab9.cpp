// Lab9.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <mpi.h>
#include <vector>
#include <time.h>
#include <chrono>
#include <fstream>
#include <stdint.h>

using namespace std;

ofstream fout("out.txt");

#define DIMENSION 64
#define TAG_METADATA 1
#define TAG_P 2
#define TAG_RESULT 3

void printPolyConsole(vector<int>& a)
{
	for (int i = 0; i < a.size(); i++)
	{
		cout << a[i];
		if (i != 0)
			cout << "x^" << i;
		if (i != a.size() - 1)
			cout << " + ";
	}
	cout << '\n';
}

void printPoly(vector<int>& a) {

	for (auto as : a) {
		fout << as << " ";
	}
	fout << '\n';
	fout.flush();
}

vector<int> increaseDegree(vector<int>& a, int deg) {
	vector<int> c;
	for (int i = 0; i < a.size(); i++) {
		c.push_back(a[i]);
	}
	for (int i = 0; i < deg; i++) {
		c.push_back(0);
	}

	return c;
}

vector<int> enlargePoly(vector<int>& a, int factor) {
	vector<int> c;
	for (int i = 0; i < factor; i++)
	{
		c.push_back(0);
	}
	for (int i = 0; i < a.size(); i++)
	{
		c.push_back(a[i]);
	}

	return c;
}

vector<int> addPoly(vector<int>& a, vector<int>& b) {
	vector<int> c;
	if (a.size() > b.size()) {
		for (size_t i = 0; i < b.size(); i++)
		{
			c.push_back(a[i] + b[i]);
		}
		for (size_t i = b.size(); i < a.size(); i++)
		{
			c.push_back(a[i]);
		}
	}
	else {
		for (size_t i = 0; i < a.size(); i++)
		{
			c.push_back(a[i] + b[i]);
		}
		for (size_t i = a.size(); i < b.size(); i++)
		{
			c.push_back(b[i]);
		}
	}

	return c;
}

vector<int> subPoly(vector<int>& a, vector<int>& b) {
	vector<int> c;
	for (size_t i = 0; i < a.size(); i++)
	{
		c.push_back(a[i] - b[i]);
	}
	return c;
}

vector<int> multiplyOne(vector<int>& a, vector<int>& b) {

	vector<int> c;
	for (size_t i = 0; i < 2 * a.size() - 1; i++)
	{
		c.push_back(0);
	}
	for (size_t i = 0; i < a.size(); i++)
	{
		for (size_t j = 0; j < b.size(); j++)
		{
			c[i + j] += a[i] * b[j];
		}
	}

	return c;
}

vector<int> multiplyMPI(vector<int>& a, vector<int>& b, int begin, int end) {

	vector<int> c(2 * a.size() - 1);

	for (int i = begin; i < end; i++)
	{
		for (int j = 0; j < b.size(); j++)
		{
			c[i + j] += a[i] * b[j];
		}
	}
	return c;
}

vector<int> computeResult(vector<int>& res1, vector<int>& res4, vector<int>& inter3, int coef, int newCoef) {
	vector<int> inter4 = subPoly(inter3, res1);
	vector<int> inter5 = subPoly(inter4, res4);

	//(P1(X)+P2(X)) * (Q1(X)+Q2(X)) - P1(X)* Q1(X) - P2(X)*Q2(X)

	res1 = increaseDegree(res1, coef);
	res4 = enlargePoly(res4, coef);
	vector<int> res6 = enlargePoly(inter5, newCoef);
	res6 = increaseDegree(res6, newCoef);

	vector<int> res5 = addPoly(res1, res4);
	vector<int> res7 = addPoly(res5, res6);
	//P*Q=P0q0+p1q1+(p0+p1)(q0+q1)-p0q0-p1q1

	printPoly(res7);
	return res7;
}

vector<int> multiplyMultiKara(vector<int>& a, vector<int>& b, int nrProcs, int me) {

	int coef = a.size();
	if (coef == 2) {
		return vector<int>{a[0] * b[0], a[0] * b[1] + a[1] * b[0], a[1] * b[1]};
	}

	vector<int> a1, a2;
	vector<int> b1, b2;
	for (int i = 0; i < coef / 2; i++)
	{
		a1.push_back(a[i]);
		a2.push_back(a[i + coef / 2]);
		b1.push_back(b[i]);
		b2.push_back(a[i + coef / 2]);

	}
	int newCoef = coef / 2;

	vector<int> inter1 = addPoly(a1, a2);
	vector<int> inter2 = addPoly(b1, b2);

	if (nrProcs >= 3) {

		int child1 = me + nrProcs / 3;
		int child2 = child1 + nrProcs / 3;
		int nrProcs0 = child1 - me;
		int nrProcs1 = child2 - child1;
		int nrProcs2 = me + nrProcs - child2;


		int metadata[2];
		metadata[0] = newCoef;
		metadata[1] = nrProcs1;

		vector<int> res1(coef), res4(coef);

		MPI_Ssend(metadata, 2, MPI_INT, child1, TAG_METADATA, MPI_COMM_WORLD);
		MPI_Ssend(a1.data(), a1.size(), MPI_INT, child1, TAG_P, MPI_COMM_WORLD);
		MPI_Ssend(b1.data(), b1.size(), MPI_INT, child1, TAG_P, MPI_COMM_WORLD);

		metadata[1] = nrProcs2;

		MPI_Ssend(metadata, 2, MPI_INT, child2, TAG_METADATA, MPI_COMM_WORLD);
		MPI_Ssend(a2.data(), a2.size(), MPI_INT, child2, TAG_P, MPI_COMM_WORLD);
		MPI_Ssend(b2.data(), b2.size(), MPI_INT, child2, TAG_P, MPI_COMM_WORLD);

		MPI_Status status;

		vector<int> inter3 = multiplyMultiKara(inter1, inter2, nrProcs0, me);

		MPI_Recv(res1.data(), res1.size(), MPI_INT, child1, TAG_RESULT, MPI_COMM_WORLD, &status);
		MPI_Recv(res4.data(), res4.size(), MPI_INT, child2, TAG_RESULT, MPI_COMM_WORLD, &status);

		return computeResult(res1, res4, inter3, coef, newCoef);

	}
	else if (nrProcs == 1) {
		vector<int> res1 = multiplyMultiKara(a1, b1, nrProcs, me);
		vector<int> res4 = multiplyMultiKara(a2, b2, nrProcs, me);
		vector<int> inter3 = multiplyMultiKara(inter1, inter2, nrProcs, me);
		return computeResult(res1, res4, inter3, coef, newCoef);
	}
	else {
		int child = me + 1;
		int metadata[2];
		metadata[0] = newCoef;
		metadata[1] = 1;
		vector<int> res1 = multiplyMultiKara(a1, b1, nrProcs / 2, me);
		vector<int> res4 = multiplyMultiKara(a2, b2, nrProcs / 2, me);
		vector<int> inter3(coef - 1);
		MPI_Status status;

		fout << me << '\n';
		fout << child << '\n';
		fout << "inter3 size before" << inter3.size() << '\n';
		fout.flush();
		MPI_Ssend(metadata, 2, MPI_INT, child, TAG_METADATA, MPI_COMM_WORLD);
		MPI_Ssend(inter1.data(), inter1.size(), MPI_INT, child, TAG_P, MPI_COMM_WORLD);
		MPI_Ssend(inter2.data(), inter2.size(), MPI_INT, child, TAG_P, MPI_COMM_WORLD);

		MPI_Recv(inter3.data(), inter3.size(), MPI_INT, child, TAG_RESULT, MPI_COMM_WORLD, &status);

		printPoly(res1);
		printPoly(res4);
		printPoly(inter3);

		fout << "inter3 size after" << inter3.size() << '\n';
		fout.flush();

		return computeResult(res1, res4, inter3, coef, newCoef);
	}

}



int main(int argc, char** argv)
{
	MPI_Init(0, 0);
	int me;
	int nrProcs;
	MPI_Comm_size(MPI_COMM_WORLD, &nrProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &me);


	if (me == 0) {
		vector<int> a, b,c;

		for (unsigned int i = 0; i < DIMENSION; i++)
		{
			a.push_back(i + 1);
			b.push_back(i + 1);
		}

		c = multiplyMultiKara(a, b, nrProcs, me);


		printPoly(c);
		printPolyConsole(c);
	}
	else {
		int metadata[2];
		MPI_Status status;
		MPI_Recv(metadata, 2, MPI_INT, MPI_ANY_SOURCE, TAG_METADATA, MPI_COMM_WORLD, &status);

		int parent = status.MPI_SOURCE;
		int size = metadata[0];
		int nProcs = metadata[1];

		fout << nProcs<<"\n\n";
		fout.flush();

		vector<int> a(size), b(size),c;

		MPI_Recv(a.data(), a.size(), MPI_INT, parent, TAG_P, MPI_COMM_WORLD, &status);
		MPI_Recv(b.data(), b.size(), MPI_INT, parent, TAG_P, MPI_COMM_WORLD, &status);

		c = multiplyMultiKara(a, b, nProcs, me);

		printPoly(c);

		MPI_Ssend(c.data(), c.size(), MPI_INT, parent, TAG_RESULT, MPI_COMM_WORLD);
	}
	//if (me == 0) {
	//	vector<int> a, b, c, intermediary(2 * DIMENSION - 1);

	//	for (unsigned int i = 0; i < DIMENSION; i++)
	//	{
	//		a.push_back(i + 1);
	//		b.push_back(i + 1);
	//	}
	//	int begin, end;
	//	begin = 0;
	//	end = DIMENSION / nrProcs;
	//	c = multiplyMPI(a, b, begin, end);
	//	//fout << "C\n";
	//	for (int i = 1; i < nrProcs; i++) {

	//		//send
	//		MPI_Ssend(a.data(), a.size(), MPI_INT, i, 1, MPI_COMM_WORLD);
	//		MPI_Ssend(b.data(), b.size(), MPI_INT, i, 2, MPI_COMM_WORLD);
	//	}

	//	MPI_Status status;
	//	for (int i = 1; i < nrProcs; i++) {

	//		//receive

	//		MPI_Recv(intermediary.data(), intermediary.size(), MPI_INT, i, 3, MPI_COMM_WORLD, &status);

	//		fout << "Intermediary\n";
	//		printPoly(intermediary);
	//		c = addPoly(c, intermediary);
	//		fout << "C after\n";
	//		printPoly(c);
	//	}

	//	printPolyConsole(c);
	//}
	//else {
	//	int begin = (DIMENSION * me) / nrProcs;
	//	int end = (DIMENSION * (me + 1)) / nrProcs;
	//	MPI_Status status;

	//	fout << "me:" << me << '\n';
	//	fout << "begin:" << begin << '\n';
	//	fout << "end:" << end << '\n';
	//	fout << '\n';
	//	fout.flush();

	//	vector<int> a(DIMENSION), b(DIMENSION), c;

	//	MPI_Recv(a.data(), a.size(), MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
	//	MPI_Recv(b.data(), b.size(), MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

	//	printPoly(a);
	//	printPoly(b);

	//	c = multiplyMPI(a, b, begin, end);

	//	printPoly(c);

	//	MPI_Ssend(c.data(), c.size(), MPI_INT, 0, 3, MPI_COMM_WORLD);
	//}
	MPI_Finalize();
}
