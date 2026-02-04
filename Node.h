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
	Node(int numFaultyNodes, bool isLoyal = true) : id(++nextId), numFaultyNodes(numFaultyNodes), isLoyal(isLoyal) {};

	int broadcastMsg(Message msg);
	void recieveMsg(Message msg);
	std::string choice();

	int getId() const { return id; }
	void setOtherNodes(std::vector<Node*> nodes) { otherNodes = nodes; }

	std::vector<uint8_t> getPrivateKey() const {return privateKey; }
	void setPrivateKey(std::vector<uint8_t> pk) { privateKey = pk;}

private:
	static int nextId;
	const int id;
	std::vector<uint8_t> privateKey;
	int numFaultyNodes;
	bool isLoyal;

	std::vector<Node*> otherNodes;
	std::set<std::string> recievedValues;

	
	
};

