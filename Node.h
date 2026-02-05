#pragma once

#include <string>
#include <vector>
#include "Message.h"
#include <map>
#include <string>
#include <set>
#include <memory>
#include <atomic>

#include "spdlog/spdlog.h"


class Node
{
public:
	Node(int numFaultyNodes, bool isLoyal = true);

	void broadcastMsg(Message msg);
	void recieveMsg(Message msg);
	std::string choice();

	int getId() const { return id; }
	void setOtherNodes(std::vector<Node*> nodes) { otherNodes = nodes; }

	std::vector<uint8_t> getPrivateKey() const { return privateKey; }
	void setPrivateKey(std::vector<uint8_t> pk) { privateKey = pk;}

	bool getIsLoyal() const { return isLoyal; }
	void setIsGeneral(bool val) { isGeneral = val; }
	std::set<std::string> getRecievedValues() const { return recievedValues; }

	// Logger access
	std::shared_ptr<spdlog::logger> getLogger() { return nodeLogger; }

private:
	static int nextId;
	const int id;
	std::vector<uint8_t> privateKey;
	int numFaultyNodes;
	bool isLoyal;
	bool isGeneral = false;

	std::vector<Node*> otherNodes;
	std::set<std::string> recievedValues;

	std::shared_ptr<spdlog::logger> nodeLogger;
};

extern std::atomic<bool> g_slowMode;

