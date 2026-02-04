#pragma once

#include <string>
#include <vector>
#include "Message.h"
#include <map>
#include <string>
#include <set>

class Node
{
public:
	Node() : id(++nextId) {};

	int getId() const { return id; }

	int broadcastMsg(Message msg);
	void recieveMsg(Message msg);
	void setOtherNodes(std::vector<Node*> nodes) { otherNodes = nodes; }

	std::string choice();
private:
	static int nextId;
	const int id;
	std::vector<Node*> otherNodes;

	std::set<std::string> recievedValues;

	
};

