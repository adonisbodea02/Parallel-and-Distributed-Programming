#include<iostream>
#include<vector>
#include<Windows.h>
#include <condition_variable>
#include <mutex>
#include <thread>


#define DIMENSION 10

using namespace std;

class Row {

public:

	mutex mtx;
	condition_variable cv;
	int row;
	int* elements;

	Row(int row) {
		this->row = row;
		this->elements = new int[DIMENSION];
		for (int i = 0; i < DIMENSION; i++)
			this->elements[i] = 0;
	}

	bool is_ready() {
		for (int i = 0; i < DIMENSION; i++) {
			if (elements[i] == 0) return false;
		}
		return true;
	}
};

class Matrix {

public:
	vector<Row*> rows;

	Matrix() {
		for (int i = 0; i < DIMENSION; i++)
			rows.push_back(new Row(i));
	}

	void print_matrix() {
		for (int i = 0; i < DIMENSION; i++)
		{
			for (int j = 0; j < DIMENSION; j++)
				cout << rows[i]->elements[j] << " ";
			cout << '\n';
		}
	}
};

void first_multiplication_by_lines(int *a, int **b, Row* c) {
	
	unique_lock<mutex> lck(c->mtx);
	for (int i = 0; i < DIMENSION; i++) {
		for (int j = 0; j < DIMENSION; j++)
			c->elements[i] += a[j] * b[j][i];
	}
	c->cv.notify_one();
	
}

void second_multiplication_by_lines(Row* a, int **b, Row* c) {
	unique_lock<mutex> lck(a->mtx);
	while (!a->is_ready())
	{
		cout << "Still waiting on line " << a->row << endl;
		a->cv.wait(lck);
	}
	for (int i = 0; i < DIMENSION; i++) {
		for (int j = 0; j < DIMENSION; j++)
			c->elements[i] += a->elements[j] * b[j][i];
	}
}

void print_matrix(int **a) {
	for (int i = 0; i < DIMENSION; i++)
	{
		for (int j = 0; j < DIMENSION; j++)
			cout << a[i][j] << " ";
		cout << '\n';
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

	Matrix ab = Matrix();
	Matrix abc = Matrix();

	for (int i = 0; i < DIMENSION; i++)
		for (int j = 0; j < DIMENSION; j++)
		{
			a[i][j] = 1;
			b[i][j] = 2;
			c[i][j] = 3;
		}

	cout << "First matrix: \n";
	print_matrix(a);
	cout << "Second matrix: \n";
	print_matrix(b);
	cout << "Third matrix: \n";
	print_matrix(c);

	vector<thread*> threads;
	vector<int> ints;

	for (int i = 0; i < DIMENSION; i++) {
		threads.push_back(new std::thread(&second_multiplication_by_lines, ab.rows[i], c, abc.rows[i]));
		threads.push_back(new std::thread(&first_multiplication_by_lines, a[i], b, ab.rows[i]));
		//threads.push_back(new std::thread(&second_multiplication_by_lines, ab.rows[i], c, abc.rows[i]));
	}

	for (unsigned int i = 0; i < threads.size(); i++)
		threads[i]->join();

	cout << "\nFirst multiplication: \n";
	ab.print_matrix();
	cout << "\nSecond multiplication: \n";
	abc.print_matrix();
	int command;
	cout << "\nPress 0 to exit: ";
	cin >> command;
}