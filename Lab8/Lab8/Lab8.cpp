#include "pch.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <future>

using namespace std;

class DirectedGraph {

public:
	vector<int>* vertices;
	vector<vector<int>*>* edges;

	DirectedGraph(int nodes) {
		vertices = new vector<int>[nodes];
		edges = new vector<vector<int>*>[nodes];

		for (int i = 0; i < nodes; i++) {
			vertices->push_back(i);
			edges->push_back(new vector<int>);
		}
	}

	void addEdge(int a, int b) {
		this->edges->at(a)->push_back(b);
	}

	vector<int>* getVertices() {
		return this->vertices;
	}

	vector<int>* getOutgoingNeighbours(int vertix) {
		return this->edges->at(vertix);
	}

	void printGraph() {
		for (int i = 0; i < this->edges->size(); i++)
		{
			cout << "Node " << i << " Outgoing Neighbours\n";
			for (int j = 0; j < this->edges->at(i)->size(); j++)
				cout << this->edges->at(i)->at(j) << '\n';
		}
	}

};

DirectedGraph generateHamiltonian(int size) {
	DirectedGraph dg = DirectedGraph(size);

	vector<int> *vertices = dg.getVertices();

	for (int i = 1; i < vertices->size(); i++) {
		dg.addEdge(i-1, i);
	}

	dg.addEdge(vertices->size() - 1, 0);

	srand((unsigned)time(0));
	for (int i = 0; i < size; i++) {
		int nodeA = rand() % size;
		int nodeB = rand() % size;
		vector<int>* neighbours = dg.getOutgoingNeighbours(nodeA);
		if (find(neighbours->begin(), neighbours->end(), nodeB) == neighbours->end())
			dg.addEdge(nodeA, nodeB);
	}

	return dg;
}

DirectedGraph generateRandom(int size) {
	cout << "Vertices: " << size << "\n";
	DirectedGraph dg = DirectedGraph(size);

	vector<int> *vertices = dg.getVertices();

	int edges = 0;

	srand((unsigned)time(0));
	int dim = rand() % size;
	for (int i = 0; i < size * dim; i++) {
		int nodeA = rand() % size;
		int nodeB = rand() % size;
		vector<int>* neighbours = dg.getOutgoingNeighbours(nodeA);
		if (find(neighbours->begin(), neighbours->end(), nodeB) == neighbours->end())
		{
			dg.addEdge(nodeA, nodeB);
			edges++;
		}
	}

	cout << "Edges: " << edges << "\n";

	return dg;
}

void printSolution(int path[], int size)
{
	cout << "Solution Exists:"
		" Following is one Hamiltonian Cycle \n";
	for (int i = 0; i < size; i++)
		cout << path[i] << " ";

	cout << path[0] << "\n";
}

bool isSafe(int v, DirectedGraph dg, int path[], int pos)
{
	vector<int>* neighbours = dg.getOutgoingNeighbours(path[pos - 1]);
	if (find(neighbours->begin(), neighbours->end(), v) == neighbours->end())
		return false;

	for (int i = 0; i < pos; i++)
		if (path[i] == v)
			return false;

	return true;
}

bool hamCycleUtil(DirectedGraph dg, int path[], int pos)
{
	if (pos == dg.vertices->size())
	{
		vector<int>* neighbours = dg.getOutgoingNeighbours(path[pos - 1]);
		if (find(neighbours->begin(), neighbours->end(), path[0]) != neighbours->end())
			return true;
		else
			return false;
	}
 
	for (int v = 0; v < dg.vertices->size(); v++)
	{
		vector<future<bool>> ftrs;
		if (isSafe(v, dg, path, pos))
		{
			path[pos] = v;

			//cout << pos << "\n";
			ftrs.push_back(async(launch::deferred, hamCycleUtil, dg, path, pos + 1));
			/*if (ftr.get() == true)
				return true;

			path[pos] = -1;*/
		}
		for (int i = 0; i < ftrs.size(); i++) {
			if (ftrs[i].get() == true)
				return true;

			path[pos] = -1;
		}
	}

	return false;
}

void hamCycle(DirectedGraph dg, int v)
{
	int *path = new int[dg.vertices->size()];
	for (int i = 0; i <= dg.vertices->size(); i++)
		path[i] = -1;

	path[0] = v;
	if (hamCycleUtil(dg, path, 1) == false)
	{
		cout << "\nSolution does not exist\n";
	}
	else
	{
		printSolution(path, dg.vertices->size());
	}
}

int main()
{
	chrono::steady_clock::time_point begin, end;
	begin = chrono::high_resolution_clock::now();
	DirectedGraph d = generateHamiltonian(25);
	d.printGraph();
	hamCycle(d, 2);
	end = chrono::high_resolution_clock::now();
	cout << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms\n";
}
