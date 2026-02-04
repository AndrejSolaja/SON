#pragma once
#include "Node.h"
#include <vector>
#include <memory>

class Simulation
{
public:
	Simulation(int N, int m, bool isGeneralLoyal) : totalNumNodes(N), numFaultyNodes(m), isGeneralLoyal(isGeneralLoyal) {};
	void init();
	void start();
	void end();

private:
	int totalNumNodes; // N
	int numFaultyNodes; // m
	bool isGeneralLoyal;

	std::vector<std::unique_ptr<Node>> nodes;
};

