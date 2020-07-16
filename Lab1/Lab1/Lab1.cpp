#include "pch.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <algorithm>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define THREAD_NUMBER 100
#define ACCOUNT_NUMBER 15

using namespace std;

class Transfer;

class Account {
public:
	static long counter;
	long id;
	atomic<int> balance;
	int initialBalance;
	mutex m;
	vector<Transfer*> transfers;

	Account(int balance) {
		this->balance = balance;
		this->id = counter++;
		this->initialBalance = balance;
	}
};

class Transfer {
public:
	static long counter;
	long id;
	Account *source;
	Account *destination;
	int amount;
	bool failure;

	Transfer(Account *source, Account *destination, int amount, bool failure) {
		this->source = source;
		this->destination = destination;
		this->amount = amount;
		this->id = counter++;
		this->failure = failure;
	}
};

bool transfer(Account* source, Account* destination, int amount, vector<Transfer*> all_transfers){
	if (source->id < destination->id) {
		source->m.lock();
		destination->m.lock();
	}
	else {
		destination->m.lock();
		source->m.lock();
	}
	if (source->balance > amount)
	{
		source->balance -= amount;
		Transfer* t = new Transfer(source, destination, amount, false);
		all_transfers.push_back(t);
		source->transfers.push_back(t);
		destination->balance += amount;
		destination->transfers.push_back(t);
		source->m.unlock();
		destination->m.unlock();
		return true;
	}
	else {
		Transfer* t = new Transfer(source, destination, amount, true);
		source->transfers.push_back(t);
		destination->transfers.push_back(t);

		source->m.unlock();
		destination->m.unlock();
		return false;
	}
}

bool check(vector<Account*> accounts, vector<Transfer*> all_transfers) {

	vector<vector<Transfer*>> logs;
	vector<int> balances;

	for (int currentAcc = 0; currentAcc < ACCOUNT_NUMBER; currentAcc++) {
		accounts[currentAcc]->m.lock();
		logs.push_back(accounts[currentAcc]->transfers);
		balances.push_back(accounts[currentAcc]->balance);
		accounts[currentAcc]->m.unlock();
	}

	cout << '\n' << "CHECKING... \n";

	for (int currentAcc = 0; currentAcc < ACCOUNT_NUMBER; currentAcc++) {
		int balance = accounts[currentAcc]->initialBalance;
		for (unsigned int i = 0; i < logs[currentAcc].size(); i++) {
			if (!logs[currentAcc][i]->failure) {
				if(logs[currentAcc][i]->source->id == accounts[currentAcc]->id)
					balance -= logs[currentAcc][i]->amount;
				else
					balance += logs[currentAcc][i]->amount;
			}
		}

		//cout << "Account " << currentAcc << " balance now " << accounts[currentAcc]->balance << " balance computed " << balance << '\n';

		if (balance != balances[currentAcc]) {
			cout << "Account " << currentAcc << " invalid balance." << '\n';
			return false;
		}
	}

	for (unsigned int currentTr = 0; currentTr < all_transfers.size(); currentTr++) {
		long sourceID = all_transfers[currentTr]->source->id;
		long destinationID = all_transfers[currentTr]->destination->id;
		vector<Transfer*>::iterator found_source;
		vector<Transfer*>::iterator found_destination;
		found_source = find(logs[sourceID].begin(), logs[sourceID].end(), all_transfers[currentTr]);
		found_destination = find(logs[destinationID].begin(), logs[destinationID].end(), all_transfers[currentTr]);
		if (found_source == logs[sourceID].end() || found_destination == logs[destinationID].end()) {
			cout << "Transaction " << all_transfers[currentTr]->id << " is not consistent in both accounts";
		}
	}
	return true;
}

long Account::counter = 0;
long Transfer::counter = 0;

int main()
{
	srand(time(nullptr));
	chrono::steady_clock::time_point start = chrono::steady_clock::now();
	vector<Account*> accounts;
	vector<thread*> threads;
	vector<Transfer*> all_transfers;

	for (int currentAcc = 0; currentAcc < ACCOUNT_NUMBER; currentAcc++) {
		accounts.push_back(new Account(rand() % 100 + 1));
		cout << "Account " << accounts.back()->id << ": " << accounts.back()->balance << "$" << '\n';
	}

	cout << "\nBEGIN:" << '\n';

	for (int currentThread = 0; currentThread < THREAD_NUMBER; currentThread++) {
		int i = 0, j = 0;
		while (i == j) {
			i = rand() % ACCOUNT_NUMBER;
			j = rand() % ACCOUNT_NUMBER;
		}
		if (i < 0) i = -i;
		if (j < 0) j = -j;

		threads.push_back(new thread(&transfer, accounts[i], accounts[j], rand() % 19 + 1, all_transfers));

		if (currentThread % 10 == 0) {
			threads.push_back(new thread(check, accounts, all_transfers));
		}
	}

	for (unsigned int iThrd = 0; iThrd < threads.size(); iThrd++) {
		threads[iThrd]->join();
	}

	for (int currentAcc = 0; currentAcc < ACCOUNT_NUMBER; currentAcc++) {
		cout << "Account " << accounts[currentAcc]->id << ": " << accounts[currentAcc]->balance << "$" << '\n';
		for (auto &transfer : accounts[currentAcc]->transfers) {
			cout << "\t#" << transfer->id << " ";
			if (transfer->failure) {
				cout << "failed to ";
			}
			if (transfer->source->id == accounts[currentAcc]->id) {
				cout << "sent " << transfer->amount << " to " << transfer->destination->id;
			}
			else {
				cout << "received " << transfer->amount << " from " << transfer->source->id;
			}
			cout << '\n';
		}
		cout << '\n';
	}

	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << "BENCHMARK: \nRunning took "
		<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
		<< "ms.\n";

	_CrtDumpMemoryLeaks();

	return 0;
}