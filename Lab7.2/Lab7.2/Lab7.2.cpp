#include "pch.h"
#include<vector>
#include<iostream>
#include<string>
#include<thread>
#include<queue>
#include<stack>
#include<algorithm>
#include "BlockingQueue.cpp"
#include<Windows.h>

#define DIMENSION 16

using namespace std;

mutex mtx;

vector<string> numbers;
vector<BlockingQueue<int>> queues{ DIMENSION + 1 };

void firstSum(int index1, int index2, int output) {
	BlockingQueue<int>* q = &(queues[output]);
	string nr1 = numbers[index1];
	string nr2 = numbers[index2];
	int i = nr1.size() - 1;
	int carry = 0;
	int remainder = 0;
	while (i >= 0) {
		int number1 = nr1[i] - '0';
		int number2 = nr2[i] - '0';
		remainder = number1 + number2;
		q->push(remainder);
		i--;
	}
	q->push(-1);
}

void waitingSum(int input1, int input2, int output) {
	BlockingQueue<int>* outQueue = &(queues[output]);
	BlockingQueue<int>* inQueue1 = &(queues[input1]);
	BlockingQueue<int>* inQueue2 = &(queues[input2]);
	int in1 = 0;
	int in2 = 0;
	int carry = 0;
	int remainder = 0;
	do {
		in1 = inQueue1->pop();
		in2 = inQueue2->pop();
		if (in1 != -1 && in2 != -1) {
			remainder = in1 + in2;
			outQueue->push(remainder);
		}
	} while (in1 != -1 && in2 != -1);
	outQueue->push(carry);
	outQueue->push(-1);
}


int main() {

	vector<thread> threads;

	for (size_t i = 0; i < DIMENSION; i++)
	{
		numbers.push_back("22222233");
	}

	for (size_t i = 1; i < DIMENSION; i++)
	{
		if (i < DIMENSION / 2) {
			threads.emplace_back(waitingSum, 2 * i, 2 * i + 1, i);
		}
		else {
			threads.emplace_back(firstSum, 2 * i - DIMENSION, 2 * i + 1 - DIMENSION, i);
		}
	}

	for (size_t i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}
	int carry = 0;
	stack<int> st;
	for (size_t i = 0; queues[1].size(); i++)
	{

		int number = queues[1].pop();
		if (number != -1) {
			number += carry;
			st.push(number % 10);
			carry = number / 10;
		}
	}
	while (st.size() > 0) {
		int number = st.top();
		st.pop();
		if (number != 0)
			cout << number;
	}
	return 0;
}