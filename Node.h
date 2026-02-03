#pragma once

#include <string>
#include <vector>
#include "Message.h"

class Node
{
public:
	Node() : id(++nextId) {};

	int getId() const { return id; }

	void broadcastMsg(const Message& msg);
	void recieveMsg(const Message& msg);
	void setOtherNodes(std::vector<Node*> nodes) { otherNodes = nodes; }

private:
	static int nextId;
	const int id;
	std::vector<Node*> otherNodes;
};

