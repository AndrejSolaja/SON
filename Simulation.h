#pragma once
#include "Node.h"
#include <vector>
#include <memory>

class Simulation
{
public:
	Simulation(int N, int m) : totalNumNodes(N), numFaultyNodes(m) {};
	void init();
	void start();

private:
	int totalNumNodes; // N
	int numFaultyNodes; // m

	std::vector<std::unique_ptr<Node>> nodes;
};

